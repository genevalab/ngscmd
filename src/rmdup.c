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

/* reverse complement the bases in a fastQ file */

int
ngs_rmdup(ngsParams *p)
{
<<<<<<< HEAD
	int i = 0;
	int j = 0;
	int buffCount = 0;
	size_t k = 0;
	size_t len = 0;
	char **seqLine;
=======
	int i, j;
	int buffCount = 0;
	char iobuff[BUFFSIZE][MAX_LINE_LENGTH];
>>>>>>> 1d4343d0315645e10717853a2f64217068cb111e
	gzFile seq;
	gzFile out;


	/* open sequence input file */
	if ((seq = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastQ sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* open sequence output file */
	if ((out = gzopen(p->outFile1, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output fastQ sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* set up interrupt trap */
	signal(SIGINT, INThandler);

	/* read through input sequence file */
	while (1)
	{
		/* initialize counter for the number of lines in the buffer */
		buffCount = 0;

		/* fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			/* get line from sequence file */
			if (gzgets(seq, iobuff[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* increment the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* reverse complement bases and reverse quality scores */
		for (i = 0; i < buffCount; ++i)
		{
			j = i % 4;
			if ((j == 1) || (j == 3))
			{
				chomp(iobuff[i]);
				strrev(iobuff[i]);
				if (j == 1)
				{
<<<<<<< HEAD
					k = 0;
					len = strlen(seqLine[i]);
=======
					size_t k = 0;
					size_t len = strlen(iobuff[i]);
>>>>>>> 1d4343d0315645e10717853a2f64217068cb111e
					while (k < len)
					{
						if (iobuff[i][k] == 'A')
							gzputc(out, 'T');
						else if (iobuff[i][k] == 'C')
							gzputc(out, 'G');
						else if (iobuff[i][k] == 'G')
							gzputc(out, 'C');
						else if (iobuff[i][k] == 'T')
							gzputc(out, 'A');
						else
							gzputc(out, iobuff[i][k]);
						++k;
					}
				}
				else
					gzputs(out, iobuff[i]);
				gzputc(out, '\n');
			}
			else
				gzputs(out, iobuff[i]);
		}

		/* if we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* close sequence input stream */
	gzclose(seq);

	/* close sequence output stream */
	gzclose(out);

	return 0;
}
