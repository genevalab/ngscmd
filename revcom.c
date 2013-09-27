/*************************************************************************
 *
 * File: revcom.c
 *
 * Description: Functions to reverse complement all sequences in a 
 *              fastq file
 *
 * Author: Daniel Garrigan
 *
 *************************************************************************/
#include "ngsutils.h"

int
ngs_revcom(ngsParams *p)
{
	int i = 0;
	char **seqLine;
	gzFile seq;
	gzFile out;


	/* Open sequence input file */
	if ((seq = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open sequence output file */
	if ((out = gzopen(p->outFile1, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Set up interrupt trap */
	signal(SIGINT, INThandler);

	/* Allocate memory for buffer */
	seqLine = (char**) malloc(BUFFSIZE * sizeof(char*));
	if (seqLine == NULL)
	{
		fputs("Memory allocation failure for seqLine.1\n", stderr);
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
	}

	/* Read through input sequence file */
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

			/* Iterate the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* Reverse complement bases and reverse quality scores */
		for (i = 0; i < buffCount; ++i)
		{
			int j = i % 4;
			if ((j == 1) || (j == 3))
			{
				char *rev;
				rev = (char*) malloc(MAX_LINE_LENGTH * sizeof(char));
				if (rev == NULL)
				{
					fputs("Memory allocation failure for rev\n", stderr);
					exit (EXIT_FAILURE);
				}
				strcpy(rev, seqLine[i]);
				STR_TRIM(rev);
				STR_REVERSE(rev);
				if (j == 1)
				{
					size_t k;
					for (k = 0; k < strlen(rev); ++k)
					{
						if (rev[k] == 'A')
							gzputc(out, 'T');
						else if (rev[k] == 'C')
							gzputc(out, 'G');
						else if (rev[k] == 'G')
							gzputc(out, 'C');
						else if (rev[k] == 'T')
							gzputc(out, 'A');
						else
							gzputc(out, rev[k]);
					}
				}
				else
					gzputs(out, rev);
				gzputc(out, '\n');
				free(rev);
			}
			else
				gzputs(out, seqLine[i]);
		}

		/* If we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* Close sequence input stream */
	gzclose(seq);

	/* Close sequence output stream */
	gzclose(out);

	/* Take out the garbage */
	for (i = 0; i < BUFFSIZE; ++i)
		free(seqLine[i]);
	free(seqLine);
	free(p);

	return 0;
}
