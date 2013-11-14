/* Copyright (c) 2013 Daniel Garrigan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Daniel Garrigan    dgarriga@bio.rochester.edu
 */

#include "ngscmd.h"

/* trims the end of reads in a single fastQ input file */

int
ngs_trim(ngsParams *p)
{
    int i = 0;
    int s = 0;
    int max = 0;
    int max_length = 0;
    size_t j = 0;
    size_t length = 0;
    int in_buffer_count = 0;
    char in_buffer[BUFFSIZE][MAX_LINE_LENGTH];
    gzFile in_fastq;
    gzFile out_fastq;


    /* open the fastQ input stream */
    if ((in_fastq = gzopen(p->seqFile1, "rb")) == Z_NULL)
    {
        fprintf(stderr, "\n\nError: cannot open the input fastQ file: %s.\n\n",
                p->seqFile1);
        exit(EXIT_FAILURE);
    }

    /* open the fastQ output stream */
    if ((out_fastq = gzopen(p->outFile1, "wb")) == Z_NULL)
    {
        fprintf(stderr, "\n\nError: cannot open the output fastQ file: %s.\n",
                p->outFile1);
        exit(EXIT_FAILURE);
    }

    /* set up interrupt trap */
    signal(SIGINT, INThandler);


    /* read through fastQ input sequence file */
    while (1)
    {
        /* initialize counter for the number of lines in the buffer */
        in_buffer_count = 0;

        /* fill up the buffer */
        while (in_buffer_count < BUFFSIZE)
        {
            /* get line from the fastQ input stream */
            if (gzgets(in_fastq, in_buffer[in_buffer_count], MAX_LINE_LENGTH)
                    == Z_NULL)
                break;

            /* iterate the counter for the number of lines
             * currently in the buffer */
            ++in_buffer_count;
        }

        /* trim the ends of the read according to
         * argmax_x{\sum_{i=x+1}^l(INT-q_i)} */
        for (i = 0; i < in_buffer_count; ++i)
        {
            if (i % 4 == 3)
            {
                s = 0;
                max = 0;
                length = strlen(in_buffer[i]) - 1;
                max_length = length;

                for (j = length - 1; j >= (size_t) (p->min_read_length); --j)
                {
                    s += p->trim_quality - (in_buffer[i][j] - 33);
                    if (s < 0)
                        break;
                    if (s > max)
                    {
                        max = s;
                        max_length = j;
                    }
                }
                in_buffer[i][max_length] = '\n';
                in_buffer[i][max_length + 1] = '\0';
                in_buffer[i - 2][max_length] = '\n';
                in_buffer[i - 2][max_length + 1] = '\0';
                gzputs(out_fastq, in_buffer[i - 3]);
                gzputs(out_fastq, in_buffer[i - 2]);
                gzputs(out_fastq, in_buffer[i - 1]);
                gzputs(out_fastq, in_buffer[i]);
            }
        }

        /* if we are at the end of the file */
        if (in_buffer_count < BUFFSIZE)
            break;
    }

    /* close the fastQ input and output streams */
    gzclose(in_fastq);
    gzclose(out_fastq);

    return 0;
}
