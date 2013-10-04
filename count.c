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

/* Count the number of reads in a fastQ file */

int
ngs_count(ngsParams *p)
{
	int n = 0;
	char line[BUFFSIZE];
	gzFile seq;


	/* open sequence file */
	if ((seq = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastQ sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* set up interrupt trap */
	signal(SIGINT, INThandler);

	/* read through input sequence file */
	while (1)
	{
		if (gzgets(seq, line, MAX_LINE_LENGTH) == Z_NULL)
			break;
		else
			++n;
	}

	if (n % 4)
		printf("There are %d fastQ records, some are incomplete\n", n / 4);
	else
		printf ("There are %d complete fastQ records in the file\n", n /4);

	/* close sequence input stream */
	gzclose(seq);

	return 0;
}
