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

/* aligned mated pairs in two fastq files */ 

int
ngs_pair(ngsParams *p)
{
	gzFile seq1;
	gzFile seq2;
	gzFile out1;
	gzFile out2;

	/* open sequence file 1 */
	if ((seq1 = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the first input fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* open sequence file 2 */
	if ((seq2 = gzopen(p->seqFile2, "rb")) == NULL)
	{
			fputs("\n\nError: cannot open the second input fastq sequence file.\n\n", stderr);
			exit(EXIT_FAILURE);
	}

	/* open the first fastq output stream */
	if ((out1 = gzopen(p->outFile1, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output stream for the first fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* open the second fastq output stream */
	if ((out2 = gzopen(p->outFile2, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output stream for the second fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* TODO: implement merging algorithm here */

	/* set up interrupt trap */
	signal(SIGINT, INThandler);

	/* close fastq 1 input stream */
	gzclose(seq1);

	/* close fastq 2 input stream */
	gzclose(seq2);

	/* close the output 1 stream */
	gzclose(out1);

	/* close the output 1 stream */
	gzclose(out2);

	/* take out the garbage */
	free(p);

	return 0;
}
