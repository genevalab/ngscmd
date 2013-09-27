/*************************************************************************
 *
 * File: bypos.c
 *
 * Description: Functions to calculate average quality score by base 
 *              position in read
 *
 * Author: Daniel Garrigan
 *
 *************************************************************************/
#include "ngsutils.h"

/* Calculate the average quality score by base position in read */
int
ngs_bypos(ngsParams *p)
{
	int i = 0;
	int max_pos = 0;
	unsigned long long int *score_sum = NULL;
	unsigned long int *num_bases = NULL;
	char **seqLine;
	gzFile seq;

	/* Open sequence file */
	if ((seq = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastq sequence file.\n\n", stderr);
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

	/* Allocate memory for score_sum array */
	score_sum = (unsigned long long int*) calloc(MAX_LINE_LENGTH, sizeof(unsigned long long int));
	if (score_sum == NULL)
	{
		fputs("Memory allocation failure for score_sum\n", stderr);
		exit (EXIT_FAILURE);
	}

	/* Allocate memory for num_bases array */
	num_bases = (unsigned long int*) calloc(MAX_LINE_LENGTH, sizeof(unsigned long int));
	if (num_bases == NULL)
	{
		fputs("Memory allocation failure for num_bases\n", stderr);
		exit (EXIT_FAILURE);
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

		/* Tally scores along each position in the sequence */
		for (i = 0; i < buffCount; ++i)
		{
			/* If we are reading the quality score line */
			if (i % 4 == 3)
			{
				size_t j;

				/* Update the maximum sequence length */
				int k;
				k = (int) (strlen(seqLine[i]) - 1);
				max_pos = k > max_pos ? k : max_pos;

				/* Update the sum of the scores at each position in the seqeucne */
				for (j = 0; j < strlen(seqLine[i]) - 1; ++j)
				{
					score_sum[j] += (unsigned long long int) (seqLine[i][j] - 33);
					++num_bases[j];
				}
			}
		}

		/* If we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* Close sequence input stream */
	gzclose(seq);

	/* Print results to STDOUT */
	puts("Position  \tNo. Bases   \tAvg. Score");
	for (i = 0; i < max_pos; ++i)
		if (num_bases[i] > 0)
			printf("%-10d\t%-12lu\t%-12.7lf\n", i + 1, num_bases[i], (double) score_sum[i] / num_bases[i]);

	/* Take out the garbage */
	for (i = 0; i < BUFFSIZE; ++i)
		free(seqLine[i]);
	free(seqLine);
	free(num_bases);
	free(score_sum);
	free(p);

	return 0;
}
