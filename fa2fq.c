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

/* join fastA and quality files into a single fastQ file */

int
ngs_fa2fq(int ifd, int iifd, int ofd)
{
	int i = 0;
	char **seqLine;
	char **qualLine;
	gzFile seq;
	gzFile qual;
	gzFile out;

	/* open the sequence file */
	if ((seq = gzdopen(ifd, "r")) == NULL)
	{
		fputs("\n\nError: cannot open the input fasta sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* open the quality file */
	if ((qual = gzdopen(iifd, "r")) == NULL)
	{
			fputs("\n\nError: cannot open the input fasta quality file.\n\n", stderr);
			exit(EXIT_FAILURE);
	}

	/* open the fastq output stream */
	if ((out = gzdopen(ofd, "w")) == NULL)
	{
		fputs("\n\nError: cannot open the output fastq sequence file.\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* set up the interrupt trap */
	signal(SIGINT, INThandler);

	/* allocate memory for the two buffers */
	seqLine = (char**) malloc(BUFFSIZE * sizeof(char*));
	if (seqLine == NULL)
	{
		fputs("Memory allocation failure for seqLine.1\n", stderr);
		exit (EXIT_FAILURE);
	}
	qualLine = (char**) malloc(BUFFSIZE * sizeof(char*));
	if (qualLine == NULL)
	{
		fputs("Memory allocation failure for qualLine.1\n", stderr);
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
		qualLine[i] = (char*) malloc(MAX_LINE_LENGTH * sizeof(char));
		if (qualLine[i] == NULL)
		{
			fputs("Memory allocation failure for qualLine.2\n", stderr);
			exit (EXIT_FAILURE);
		}
	}

	/* read through both files */
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

			/* get line from quality file */
			if (gzgets(qual, qualLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* increment the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* dump the buffer to the output stream */
		for (i = 0; i < buffCount; ++i)
		{
			if (i % 2)
			{
				gzputs(out, seqLine[i]);
				gzputs(out, "+\n");
				gzputs(out, qualLine[i]);
			}
			else
			{
				gzputc(out, '@');
				gzputs(out, seqLine[i] + 1);
			}
		}

		/* if we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* close sequence input stream */
	gzclose(seq);

	/* close quality input stream */
	gzclose(qual);

	/* close the output stream */
	gzclose(out);

	/* take out the garbage */
	for (i = 0; i < BUFFSIZE; ++i)
	{
		free(seqLine[i]);
		free(qualLine[i]);
	}
	free(seqLine);
	free(qualLine);

	return 0;
}
