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

/* filters out low quality reads from either one fastQ or two mated fastQ files */

int
ngs_filter(ngsParams *p)
{
	int i = 0;
	int j = 0;
	int buffCount = 0;
	int count_N1 = 0;
	int count_N2 = 0;
	char iobuff1[BUFFSIZE][MAX_LINE_LENGTH];
	char iobuff2[BUFFSIZE][MAX_LINE_LENGTH];
	gzFile seq1;
	gzFile seq2;
	gzFile out1;
	gzFile out2;

	/* open the first fastQ input stream */
	if ((seq1 = gzopen(p->seqFile1, "r")) == NULL)
	{
		fprintf(stderr, "\n\nError: cannot open the input fastQ file: %s.\n\n", p->seqFile1);
		exit(EXIT_FAILURE);
	}

	/* if specified-- open the second fastQ input stream */
	if (p->flag & TWO_INPUTS)
	{
		if ((seq2 = gzopen(p->seqFile2, "r")) == NULL)
		{
				fprintf(stderr, "\n\nError: cannot open the second input fastQ file: %s.\n\n", p->seqFile2);
				exit(EXIT_FAILURE);
		}
	}

	/* open the first fastQ output stream */
	if ((out1 = gzopen(p->outFile1, "w")) == NULL)
	{
		fprintf(stderr, "\n\nError: cannot open the output fastQ file: %s.\n", p->outFile1);
		exit(EXIT_FAILURE);
	}

	/* if specified-- open the second fastQ output stream */
	if (p->flag & TWO_INPUTS)
	{
		if ((out2 = gzopen(p->outFile1, "w")) == NULL)
		{
			fprintf(stderr, "\n\nError: cannot open the second output fastQ file: %s.\n", p->outFile2);
			exit(EXIT_FAILURE);
		}
	}

	/* set up the interrupt trap */
	signal(SIGINT, INThandler);


	/* read through both files */
	while (1)
	{
		/* initialize counter for the number of lines in the buffer */
		buffCount = 0;

		/* fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			/* get line from the first fastQ input stream */
			if (gzgets(seq1, iobuff1[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* if specified-- get line from the second fastQ input stream */
			if (p->flag & TWO_INPUTS)
			{
				if (gzgets(seq2, iobuff2[buffCount], MAX_LINE_LENGTH) == Z_NULL)
					break;
			}

			/* increment the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* screen each sequence for number of ambiguous characters */
		/* if record passes filter-- write to the output streams */
		for (i = 0; i < buffCount; ++i)
		{
			if (i % 4 == 1)
			{
				count_N1 = 0;
				count_N2 = 0;

				/* count number of ambiguous characters in the first fastQ entry */
				for (j = 0; iobuff1[i][j]; j++)
					count_N1 += (iobuff1[i][j] == 'N');

				/* if specified-- count number of ambiguous characters in the second fastQ entry */
				if (p->flag & TWO_INPUTS)
				{
					for (j = 0; iobuff2[i][j]; j++)
						count_N2 += (iobuff2[i][j] == 'N');
				}

				if ((count_N1 <= p->num_ambig) || (count_N2 <= p->num_ambig))
				{
					gzputs(out1, iobuff1[i-1]);
					gzputs(out1, iobuff1[i]);
					gzputs(out1, iobuff1[i+1]);
					gzputs(out1, iobuff1[i+2]);
					if (p->flag & TWO_INPUTS)
					{
						gzputs(out2, iobuff2[i-1]);
						gzputs(out2, iobuff2[i]);
						gzputs(out2, iobuff2[i+1]);
						gzputs(out2, iobuff2[i+2]);
					}
				}
			}

		}

		/* if we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* close the first fastQ input and output streams */
	gzclose(seq1);
	gzclose(out1);

	/* if specified-- close the second fastQ input and output streams */
	if (p->flag & TWO_INPUTS)
	{
		gzclose(seq2);
		gzclose(out2);
	}

	return 0;
}
