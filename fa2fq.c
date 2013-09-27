/*************************************************************************
 *
 * File: fa2fq.c
 *
 * Description: Functions convert between fasta/quality files and
 *              fastq files
 *
 * Author: Daniel Garrigan
 *
 *************************************************************************/
#include "ngsutils.h"

/* Join fasta/quality files into a single fastq file */
int
ngs_fa2fq(ngsParams *p)
{
	int i = 0;
	char **seqLine;
	char **qualLine;
	gzFile seq;
	gzFile qual;
	gzFile out;

	/* Open the sequence file */
	if ((seq = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fasta sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open the quality file */
	if ((qual = gzopen(p->qualFile, "rb")) == NULL)
	{
			fputs("\n\nError: cannot open the input fasta quality file.\n\n", stderr);
			exit(EXIT_FAILURE);
	}

	/* Open the fastq output stream */
	if ((out = gzopen(p->outFile1, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output fastq sequence file.\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Set up the interrupt trap */
	signal(SIGINT, INThandler);

	/* Allocate memory for the two buffers */
	seqLine = (char**) malloc(BUFFSIZE * sizeof(char*));
	if (seqLine == NULL)
	{
		fputs("Memory allocation failure for seqLine.1\n", stderr);
		exit (EXIT_FAILURE);
	}
	qualLine = (char**) malloc(BUFFSIZE * sizeof(char*));
	if (qualLine == NULL)
	{
		fputs("Memory allocation failure for qualLine.1\n", stderr);
		exit (EXIT_FAILURE);
	}
	for (i = 0; i < BUFFSIZE; ++i)
	{
		seqLine[i] = (char*) malloc(MAX_LINE_LENGTH * sizeof(char));
		if (seqLine[i] == NULL)
		{
			fputs("Memory allocation failure for seqLine.2\n", stderr);
			exit (EXIT_FAILURE);
		}
		qualLine[i] = (char*) malloc(MAX_LINE_LENGTH * sizeof(char));
		if (qualLine[i] == NULL)
		{
			fputs("Memory allocation failure for qualLine.2\n", stderr);
			exit (EXIT_FAILURE);
		}
	}

	/* Read through both files */
	while (1)
	{
		/* Initialize counter for the number of lines in the buffer */
		int buffCount = 0;

		/* Fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			/* Get line from sequence file */
			if (gzgets(seq, seqLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* Get line from quality file */
			if (gzgets(qual, qualLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* Iterate the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* Dump buffer to output stream */
		for (i = 0; i < buffCount; ++i)
		{
			if (i % 2)
			{
				gzputs(out, seqLine[i]);
				gzputs(out, "+\n");
				gzputs(out, qualLine[i]);
			}
			else
			{
				gzputc(out, '@');
				gzputs(out, seqLine[i] + 1);
			}
		}

		/* If we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* Close sequence input stream */
	gzclose(seq);

	/* Close quality input stream */
	gzclose(qual);

	/* Close the output stream */
	gzclose(out);

	/* Take out the garbage */
	for (i = 0; i < BUFFSIZE; ++i)
	{
		free(seqLine[i]);
		free(qualLine[i]);
	}
	free(seqLine);
	free(qualLine);
	free(p);

	return 0;
}
