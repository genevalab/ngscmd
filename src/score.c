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

#include "ngscmd.h"

/* transform Phred-scaled quality scores in a fastQ input file */

int
ngs_score(ngsParams *p)
{
	int i = 0;
	int buffCount = 0;
	char iobuff[BUFFSIZE][MAX_LINE_LENGTH];
	gzFile seq;
	gzFile out;

	/* open the fastQ input stream */
	if ((seq = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fprintf(stderr, "\n\nError: cannot open the input fastQ file: %s.\n\n", p->seqFile1);
		exit(EXIT_FAILURE);
	}

	/* open the fastQ output stream */
	if ((out = gzopen(p->outFile1, "wb")) == NULL)
	{
		fprintf(stderr, "\n\nError: cannot open the output fastQ file: %s.\n", p->outFile1);
		exit(EXIT_FAILURE);
	}

	/* set up interrupt trap */
	signal(SIGINT, INThandler);

	/* read through fastQ input sequence file */
	while (1)
	{
		buffCount = 0;

		/* fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			if (gzgets(seq, iobuff[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;
			++buffCount;
		}

		/* dump the buffer to the output stream */
		for (i = 0; i < buffCount; ++i)
		{
			if (i % 4 == 3)
			{
				size_t j = 0;
				size_t len = strlen(iobuff[i]) - 1;
				if (p->flag & SCORE_ILLUMINA)
				{
					/* only do Sanger to Illumina conversion */
					while (j < len)
					{
						int score = iobuff[i][j] + 31;
						if (score > SCHAR_MAX)
						{
							fputs("\n\nError: the original Phred scores are not in standard Sanger format.\n\n", stderr);
							exit(EXIT_FAILURE);
						}
						else
							gzputc(out, score);
						++j;
					}
					gzputc(out, '\n');

					if (p->flag & SCORE_ASCII)
					{
						/* do both numerical and Sanger to Illumina conversion here */
						const char delim = ' ';
						char *tok;
						int score;
						tok = strtok(iobuff[i], &delim);
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
					/* only do Illumina to Sanger conversion */
					while (j < len)
					{
						int score;
						score = iobuff[i][j] - 31;
						if ((score > SCHAR_MAX) || (score < 33))
						{
							fputs("\n\nError: the original Phred scores are not in Illumina format.\n\n", stderr);
							exit(EXIT_FAILURE);
						}
						else
							gzputc(out, iobuff[i][j] - 31);
						++j;
					}
					gzputc(out, '\n');

					if (p->flag & SCORE_ASCII)
					{
						/* do both numerical and Illumina to Sanger conversion here */
						const char delim = ' ';
						char *tok;
						int score;
						tok = strtok(iobuff[i], &delim);
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
				gzputs(out, iobuff[i]);
		}

		/* if we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* close the fastQ input and output streams */
	gzclose(seq);
	gzclose(out);

	return 0;
}
