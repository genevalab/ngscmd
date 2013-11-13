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

/* filters out low quality reads from two mated fastQ files */

int
ngs_filter(ngsParams *p)
{
	int i = 0;
	int buffCount = 0;
	char **seqLine1 = NULL;
	char **seqLine2 = NULL;
	gzFile seq1;
	gzFile seq2;
	gzFile out1;
	gzFile out2;

	/* open the fastQ mate 1 file */
	if ((seq1 = gzopen(p->seqFile1, "r")) == NULL)
	{
		fprintf(stderr, "\n\nError: cannot open the input fastA sequence file: %s.\n\n", p->seqFile1);
		exit(EXIT_FAILURE);
	}

	if (p->flag & TWO_INPUTS)
	{
		/* open the fastQ mate 2 file */
		if ((seq2 = gzopen(p->seqFile2, "r")) == NULL)
		{
				fprintf(stderr, "\n\nError: cannot open the input fastA quality file: %s.\n\n", p->seqFile2);
				exit(EXIT_FAILURE);
		}
	}

	/* open the fastq mate 1 output stream */
	if ((out1 = gzopen(p->outFile1, "w")) == NULL)
	{
		fprintf(stderr, "\n\nError: cannot open the output fastq sequence file: %s.\n", p->outFile1);
		exit(EXIT_FAILURE);
	}

	if (p->flag & TWO_INPUTS)
	{
		/* open the fastq mate 2 output stream */
		if ((out2 = gzopen(p->outFile1, "w")) == NULL)
		{
			fprintf(stderr, "\n\nError: cannot open the output fastq sequence file: %s.\n", p->outFile2);
			exit(EXIT_FAILURE);
		}
	}

	/* set up the interrupt trap */
	signal(SIGINT, INThandler);

	/* allocate memory for the I/O buffers */
	seqLine1 = (char**) malloc(BUFFSIZE * sizeof(char*));
	if (seqLine1 == NULL)
	{
		fputs("Memory allocation failure for seqLine.1\n", stderr);
		exit (EXIT_FAILURE);
	}

	if (p->flag & TWO_INPUTS)
	{
		seqLine2 = (char**) malloc(BUFFSIZE * sizeof(char*));
		if (seqLine2 == NULL)
		{
			fputs("Memory allocation failure for qualLine.1\n", stderr);
			exit (EXIT_FAILURE);
		}
	}

	for (i = 0; i < BUFFSIZE; ++i)
	{
		seqLine1[i] = (char*) malloc(MAX_LINE_LENGTH * sizeof(char));
		if (seqLine1[i] == NULL)
		{
			fputs("Memory allocation failure for seqLine.2\n", stderr);
			exit (EXIT_FAILURE);
		}

		if (p->flag & TWO_INPUTS)
		{
			seqLine2[i] = (char*) malloc(MAX_LINE_LENGTH * sizeof(char));
			if (seqLine2[i] == NULL)
			{
				fputs("Memory allocation failure for qualLine.2\n", stderr);
				exit (EXIT_FAILURE);
			}
		}
	}

	/* read through both files */
	while (1)
	{
		/* initialize counter for the number of lines in the buffer */
		buffCount = 0;

		/* fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			/* get line from fastQ mate 1 file */
			if (gzgets(seq1, seqLine1[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			if (p->flag & TWO_INPUTS)
			{
				/* get line from fastQ mate 2 file */
				if (gzgets(seq2, seqLine2[buffCount], MAX_LINE_LENGTH) == Z_NULL)
					break;
			}

			/* increment the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* dump the buffer to the output stream */
		for (i = 0; i < buffCount; ++i)
		{
			/* TODO: Something here */
		}

		/* if we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* close fastQ mate 1 input stream */
	gzclose(seq1);

	/* close the output mate 1 stream */
	gzclose(out1);

	if (p->flag & TWO_INPUTS)
	{
		/* close fastQ mate 2 input stream */
		gzclose(seq2);

		/* close the output mate 2 stream */
		gzclose(out2);
	}

	/* take out the garbage */
	for (i = 0; i < BUFFSIZE; ++i)
	{
		free(seqLine1[i]);
		if (p->flag & TWO_INPUTS)
			free(seqLine2[i]);
	}
	free(seqLine1);
	if (p->flag & TWO_INPUTS)
		free(seqLine2);

	return 0;
}
