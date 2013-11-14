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

/* aligned mated pairs in two fastQ input files */

int
ngs_pair(ngsParams *p)
{
    int i = 0;
    int in_buffer_count = 0;
    char in_buffer1[BUFFSIZE][MAX_LINE_LENGTH];
    char in_buffer2[BUFFSIZE][MAX_LINE_LENGTH];
    gzFile in_fastq1;
    gzFile in_fastq2;
    gzFile out_fastq1;
    gzFile out_fastq2;

    /* open the first fastQ input stream */
    if ((in_fastq1 = gzopen(p->seqFile1, "rb")) == Z_NULL)
    {
        fprintf(stderr, "\n\nError: cannot open the input fastQ file: %s.\n\n",
                p->seqFile1);
        exit(EXIT_FAILURE);
    }

    /* open the second fastQ input stream */
    if ((in_fastq2 = gzopen(p->seqFile2, "rb")) == Z_NULL)
    {
        fprintf(stderr, "\n\nError: cannot open the second input "
                "fastQ file: %s.\n\n", p->seqFile2);
        exit(EXIT_FAILURE);
    }

    /* open the first fastQ output stream */
    if ((out_fastq1 = gzopen(p->outFile1, "wb")) == Z_NULL)
    {
        fprintf(stderr, "\n\nError: cannot open the output fastQ file: "
                "%s.\n", p->outFile1);
        exit(EXIT_FAILURE);
    }

    /* open the second fastQ output stream */
    if ((out_fastq2 = gzopen(p->outFile2, "wb")) == Z_NULL)
    {
        fprintf(stderr, "\n\nError: cannot open the second output fastQ file: "
                "%s.\n", p->outFile2);
        exit(EXIT_FAILURE);
    }

    /* set up interrupt trap */
    signal(SIGINT, INThandler);

    /* TODO: implement merging algorithm here */

    /* close the fastQ input streams */
    gzclose(in_fastq1);
    gzclose(in_fastq2);

    /* close the fastQ output streams */
    gzclose(out_fastq1);
    gzclose(out_fastq2);

    return 0;
}
