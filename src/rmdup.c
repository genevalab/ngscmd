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

/* remove duplicate sequences in one or two fastQ input files */

int
ngs_rmdup(ngsParams *p)
{
	int i = 0;
	int j = 0;
	int in_buffer_count = 0;
	size_t k = 0;
	size_t len = 0;
	char in_buffer1[BUFFSIZE][MAX_LINE_LENGTH];
	char in_buffer2[BUFFSIZE][MAX_LINE_LENGTH];
	gzFile in_fastq1;
	gzFile in_fastq2;
	gzFile out_fastq1;
	gzFile out_fastq2;


	/* open the first fastQ input stream */
	if ((in_fastq1 = gzopen(p->seqFile1, "rb")) == Z_NULL)
	{
		fprintf(stderr, "\n\nError: cannot open the input fastQ file: %s.\n\n", p->seqFile1);
		exit(EXIT_FAILURE);
	}

	/* if specified-- open the second fastQ input stream */
	if (p->flag & TWO_INPUTS)
	{
		if ((in_fastq2 = gzopen(p->seqFile2, "r")) == Z_NULL)
		{
				fprintf(stderr, "\n\nError: cannot open the second input fastQ file: %s.\n\n", p->seqFile2);
				exit(EXIT_FAILURE);
		}
	}

	/* open the first fastQ output stream */
	if ((out_fastq1 = gzopen(p->outFile1, "wb")) == Z_NULL)
	{
		fprintf(stderr, "\n\nError: cannot open the output fastQ file: %s.\n", p->outFile1);
		exit(EXIT_FAILURE);
	}

	/* if specified-- open the second fastQ output stream */
	if (p->flag & TWO_INPUTS)
	{
		if ((out_fastq2 = gzopen(p->outFile1, "w")) == Z_NULL)
		{
			fprintf(stderr, "\n\nError: cannot open the second output fastQ file: %s.\n", p->outFile2);
			exit(EXIT_FAILURE);
		}
	}

	/* set up interrupt trap */
	signal(SIGINT, INThandler);

	/* read through input fastQ files */
	while (1)
	{
		/* initialize counter for the number of lines in the buffer */
		in_buffer_count = 0;

		/* fill up the buffer */
		while (in_buffer_count < BUFFSIZE)
		{
			/* get line from first fastQ input stream */
			if (gzgets(in_fastq1, in_buffer1[in_buffer_count], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* get line from second fastQ input stream */
			if (p->flag & TWO_INPUTS)
			{
				if (gzgets(in_fastq2, in_buffer2[in_buffer_count], MAX_LINE_LENGTH) == Z_NULL)
					break;
			}

			/* increment the counter for the number of lines currently in the buffer */
			++in_buffer_count;
		}

		/* reverse complement bases and reverse quality scores */
		for (i = 0; i < in_buffer_count; ++i)
		{
			/* TODO: Remove duplicates here */
		}

		/* if we are at the end of the file */
		if (in_buffer_count < BUFFSIZE)
			break;
	}

	/* close the first fastQ input and output streams */
	gzclose(in_fastq1);
	gzclose(out_fastq1);

	/* if specified-- close the second fastQ input and output streams */
	if (p->flag & TWO_INPUTS)
	{
		gzclose(in_fastq2);
		gzclose(out_fastq2);
	}

	return 0;
}
