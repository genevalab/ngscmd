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

/* filters reads and bases in a fastQ file according to quality scores */

int
ngs_clean(ngsParams *p)
{
	int i = 0;
	char **seqLine;
	gzFile seq;
	gzFile out;

	/* open sequence input file */
	if ((seq = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* open sequence output file */
	if ((out = gzopen(p->outFile1, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output fastq sequence file.\n", stderr);
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

	/* read through input sequence file */
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

		/* tally scores along each position in the sequence */
		for (i = 0; i < buffCount; ++i)
		{
			/* TODO: add code to filter and trim sequences here */
		}

		/* if we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* close sequence input stream */
	gzclose(seq);

	/* take out the garbage */
	for (i = 0; i < BUFFSIZE; ++i)
		free(seqLine[i]);
	free(seqLine);
	free(p);

	return 0;
}
