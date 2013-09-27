/*************************************************************************
 *
 * File: convert.c
 *
 * Description: Functions to transform Phred scaled quality scores in 
 *              fastq files
 *
 * Author: Daniel Garrigan
 *
 *************************************************************************/
#include "ngsutils.h"

/* Transform Phred scaled quality scores */
int
ngs_convert(ngsParams *p)
{
	int i = 0;
	char **seqLine;
	gzFile seq;
	gzFile out;

	/* Open sequence file */
	if ((seq = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open output fastq stream */
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

	/* Read through fastq input sequence file */
	while (1)
	{
		int buffCount = 0;

		/* Fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			if (gzgets(seq, seqLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;
			++buffCount;
		}

		/* Dump buffer to output stream */
		for (i = 0; i < buffCount; ++i)
		{
			if (i % 4 == 3)
			{
				size_t j;
				if (p->flag & CONVERT_REV)
				{
					/* Only do Sanger to Illumina conversion */
					for (j = 0; j < strlen(seqLine[i]) - 1; ++j)
					{
						int score;
						score = seqLine[i][j] + 31;
						if (score > SCHAR_MAX)
						{
							fputs("\n\nError: the original Phred scores are not in standard Sanger format.\n\n", stderr);
							exit(EXIT_FAILURE);
						}
						else
							gzputc(out, score);
					}
					gzputc(out, '\n');

					if (p->flag & CONVERT_NUM)
					{
						/* Do both numerical and Sanger to Illumina conversion here */
						const char delim = ' ';
						char *tok;
						int score;
						tok = strtok(seqLine[i], &delim);
						score = atoi(tok);
						gzputc(out, score);
						while (tok != NULL)
						{
							tok = strtok(NULL, &delim);
							score = atoi(tok);
							gzputc(out, score + 31);
						}
						gzputc(out, '\n');
					}
				}
				else
				{
					/* Only do Illumina to Sanger conversion */
					for (j = 0; j < strlen(seqLine[i]) - 1; ++j)
					{
						int score;
						score = seqLine[i][j] - 31;
						if ((score > SCHAR_MAX) || (score < 33))
						{
							fputs("\n\nError: the original Phred scores are not in Illumina format.\n\n", stderr);
							exit(EXIT_FAILURE);
						}
						else
							gzputc(out, seqLine[i][j] - 31);
					}
					gzputc(out, '\n');

					if (p->flag & CONVERT_NUM)
					{
						/* Do both numerical and Illumina to Sanger conversion here */
						const char delim = ' ';
						char *tok;
						int score;
						tok = strtok(seqLine[i], &delim);
						score = atoi(tok);
						gzputc(out, score);
						while (tok != NULL)
						{
							tok = strtok(NULL, &delim);
							score = atoi(tok);
							gzputc(out, score - 31);
						}
						gzputc(out, '\n');
					}
				}
			}
			else
				gzputs(out, seqLine[i]);
		}

		/* If we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* Close fastq input sequence file stream */
	gzclose(seq);

	/* Close fastq output sequence file stream */
	gzclose(out);

	/* Take out the garbage */
	for (i = 0; i < BUFFSIZE; ++i)
		free(seqLine[i]);
	free(seqLine);
	free(p);

	return 0;
}
