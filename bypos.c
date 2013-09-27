/* Copyright (c) 2013 Daniel Garrigan

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
	the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
	FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
	IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	Daniel Garrigan    dgarriga@bio.rochester.edu
*/

#include "ngslib.h"

/* calculates the average quality score by base position in read */

int
ngs_bypos(ngsParams *p)
{
	int i = 0;
	int max_pos = 0;
	unsigned long long int *score_sum = NULL;
	unsigned long int *num_bases = NULL;
	char **seqLine;
	gzFile seq;

	/* open sequence file */
	if ((seq = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* set up interrupt trap */
	signal(SIGINT, INThandler);

	/* allocate memory for buffer */
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

	/* allocate memory for score_sum array */
	score_sum = (unsigned long long int*) calloc(MAX_LINE_LENGTH, sizeof(unsigned long long int));
	if (score_sum == NULL)
	{
		fputs("Memory allocation failure for score_sum\n", stderr);
		exit (EXIT_FAILURE);
	}

	/* allocate memory for num_bases array */
	num_bases = (unsigned long int*) calloc(MAX_LINE_LENGTH, sizeof(unsigned long int));
	if (num_bases == NULL)
	{
		fputs("Memory allocation failure for num_bases\n", stderr);
		exit (EXIT_FAILURE);
	}

	/* read through the input fastQ sequence file */
	while (1)
	{
		/* initialize counter for the number of lines in the buffer */
		int buffCount = 0;

		/* fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			/* get line from sequence file */
			if (gzgets(seq, seqLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* increment the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* tally the scores along each position in the sequence */
		for (i = 0; i < buffCount; ++i)
		{
			/* if we are reading the quality score line */
			if (i % 4 == 3)
			{
				size_t j = 0;
				size_t len = strlen(seqLine[i]) - 1;

				/* update the maximum sequence length */
				int k = (int) (len);
				max_pos = k > max_pos ? k : max_pos;

				/* update the sum of the scores at each position in the seqeucne */
				while (j < len)
				{
					score_sum[j] += (unsigned long long int) (seqLine[i][j] - 33);
					++num_bases[j];
					++j;
				}
			}
		}

		/* if we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* close sequence input stream */
	gzclose(seq);

	/* print the results to STDOUT */
	puts("Position  \tNo. Bases   \tAvg. Score");
	for (i = 0; i < max_pos; ++i)
		if (num_bases[i] > 0)
			printf("%-10d\t%-12lu\t%-12.7lf\n", i + 1, num_bases[i], (double) score_sum[i] / num_bases[i]);

	/* take out the garbage */
	for (i = 0; i < BUFFSIZE; ++i)
		free(seqLine[i]);
	free(seqLine);
	free(num_bases);
	free(score_sum);

	return 0;
}
