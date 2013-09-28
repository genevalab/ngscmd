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

/* transform Phred-scaled quality scores in a fastQ file */

int
ngs_convert(int ifd, int ofd)
{
	int i = 0;
	char **seqLine;
	gzFile seq;
	gzFile out;

	/* open sequence file */
	if ((seq = gzdopen(ifd, "r")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* open output fastq stream */
	if ((out = gzdopen(ofd, "w")) == NULL)
	{
		fputs("\n\nError: cannot open the output fastq sequence file.\n\n", stderr);
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

	/* read through fastQ input sequence file */
	while (1)
	{
		int buffCount = 0;

		/* fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			if (gzgets(seq, seqLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;
			++buffCount;
		}

		/* dump the buffer to the output stream */
		for (i = 0; i < buffCount; ++i)
		{
			if (i % 4 == 3)
			{
				size_t j = 0;
				size_t len = strlen(seqLine[i]) - 1;
				if (p->flag & CONVERT_REV)
				{
					/* only do Sanger to Illumina conversion */
					while (j < len)
					{
						int score = seqLine[i][j] + 31;
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

					if (p->flag & CONVERT_NUM)
					{
						/* do both numerical and Sanger to Illumina conversion here */
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
					/* only do Illumina to Sanger conversion */
					while (j < len)
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
						++j;
					}
					gzputc(out, '\n');

					if (p->flag & CONVERT_NUM)
					{
						/* do both numerical and Illumina to Sanger conversion here */
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

	return 0;
}
