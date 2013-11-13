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

/* counts the number of unique k-mers in a single fastQ input file
 * writes output of kmer counts to STDOUT
 */

int
ngs_kmer(ngsParams *p)
{
	int i = 0;
	int buffCount = 0;
	char iobuff[BUFFSIZE][MAX_LINE_LENGTH];
	gzFile seq;


	/* open the first fastQ input stream */
	if ((seq = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fprintf(stderr, "\n\nError: cannot open the input fastQ file: %s.\n\n", p->seqFile1);
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
			/* get line from the fastQ input stream */
			if (gzgets(seq, iobuff[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* iterate the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* tally scores along each position in the sequence */
		for (i = 0; i < buffCount; ++i)
		{
			/* TODO: implement kmer counting algorithm */
		}

		/* if we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* close the fastQ input stream */
	gzclose(seq);

	return 0;
}
