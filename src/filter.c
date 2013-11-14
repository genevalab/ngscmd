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

/* ngs_filter() -- filters out low quality reads from either 
 * one fastQ or two mated fastQ files */

int
ngs_filter(ngsParams *p)
{
	int i = 0;
	int j = 0;
	int in_buff_count = 0;
	int count_N1 = 0;
	int count_N2 = 0;
	char *ret1 = NULL;
	char *ret2 = NULL;
	char in_buffer1[BUFFSIZE][MAX_LINE_LENGTH];
	char in_buffer2[BUFFSIZE][MAX_LINE_LENGTH];
	gzFile in_fastq1;
	gzFile in_fastq2;
	gzFile out_fastq1;
	gzFile out_fastq2;

	/* open the first fastQ input stream */
	if ((in_fastq1 = gzopen(p->seqFile1, "r")) == Z_NULL)
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
	if ((out_fastq1 = gzopen(p->outFile1, "w")) == Z_NULL)
	{
		fprintf(stderr, "\n\nError: cannot open the output fastQ file: %s.\n", p->outFile1);
		exit(EXIT_FAILURE);
	}

	/* if specified-- open the second fastQ output stream */
	if (p->flag & TWO_INPUTS)
	{
		if ((out_fastq2 = gzopen(p->outFile2, "w")) == Z_NULL)
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
		in_buff_count = 0;

		/* fill up the buffer */
		while (in_buff_count < BUFFSIZE)
		{
			/* get line from the first fastQ input stream */
			ret1 = gzgets(in_fastq1, in_buffer1[in_buff_count], MAX_LINE_LENGTH);

			/* if specified-- get line from the second fastQ input stream */
			if (p->flag & TWO_INPUTS)
				ret2 = gzgets(in_fastq2, in_buffer2[in_buff_count], MAX_LINE_LENGTH);

			/* determine whether gzgets was successful */
			if (p->flag & TWO_INPUTS)
			{
				/* both gzgets calls return null-- we are at the end of the file */
				if ((ret1 == Z_NULL) && (ret2 == Z_NULL))
					break;
				/* one or the other gzgets calls returns null-- unequal number of records */
				else if (((ret1 == Z_NULL) && (ret2 != Z_NULL)) || ((ret1 != Z_NULL) && (ret2 == Z_NULL)))
				{
					fputs("Error processing paired fastQ sequences. Are these files properly paired?", stderr);
					exit(EXIT_FAILURE);
				}
			}
			else if (ret1 == Z_NULL)
				break;

			/* increment the counter for the number of lines
			 * currently in the buffer */
			++in_buff_count;
		}

		/* screen each sequence for number of ambiguous characters
		 * if record passes filter-- write to the output streams */
		for (i = 0; i < in_buff_count; ++i)
		{
			if (i % 4 == 1)
			{
				/* reset ambiguous character counters to zero */
				count_N1 = 0;
				count_N2 = 0;

				/* count the number of ambiguous characters 
				 * in the first fastQ entry */
				for (j = 0; in_buffer1[i][j]; j++)
					count_N1 += (in_buffer1[i][j] == 'N');

				/* if specified-- count the number of ambiguous characters 
				 *in the second fastQ entry */
				if (p->flag & TWO_INPUTS)
					for (j = 0; in_buffer2[i][j]; j++)
						count_N2 += (in_buffer2[i][j] == 'N');

				/* if two fastQ input streams are specified and both sequences 
				 * pass the for ambiguous characters, then write to both fastQ 
				 * output streams */
				if (p->flag & TWO_INPUTS)
				{
					if ((count_N1 <= p->num_ambig) && (count_N2 <= p->num_ambig))
					{
						gzputs(out_fastq1, in_buffer1[i-1]);
						gzputs(out_fastq1, in_buffer1[i]);
						gzputs(out_fastq1, in_buffer1[i+1]);
						gzputs(out_fastq1, in_buffer1[i+2]);
						gzputs(out_fastq2, in_buffer2[i-1]);
						gzputs(out_fastq2, in_buffer2[i]);
						gzputs(out_fastq2, in_buffer2[i+1]);
						gzputs(out_fastq2, in_buffer2[i+2]);
					}
				}
				else
				{
					if (count_N1 <= p->num_ambig)
					{
						gzputs(out_fastq1, in_buffer1[i-1]);
						gzputs(out_fastq1, in_buffer1[i]);
						gzputs(out_fastq1, in_buffer1[i+1]);
						gzputs(out_fastq1, in_buffer1[i+2]);
					}
				}
			}
		}

		/* if we are at the end of the file */
		if (in_buff_count < BUFFSIZE)
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
