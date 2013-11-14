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

/* transform Phred-scaled quality scores in a fastQ input file */

int
ngs_score(ngsParams *p)
{
    int i = 0;
    size_t j = 0;
    size_t length = 0;
    int in_buffer_count = 0;
    char in_buffer[BUFFSIZE][MAX_LINE_LENGTH];
    gzFile in_fastq;
    gzFile out_fastq;

    /* open the fastQ input stream */
    if ((in_fastq = gzopen(p->seqFile1, "rb")) == Z_NULL)
    {
        fprintf(stderr, "\n\nError: cannot open the input fastQ file: "
                "%s.\n\n", p->seqFile1);
        exit(EXIT_FAILURE);
    }

    /* open the fastQ output stream */
    if ((out_fastq = gzopen(p->outFile1, "wb")) == Z_NULL)
    {
        fprintf(stderr, "\n\nError: cannot open the output fastQ file: "
                "%s.\n", p->outFile1);
        exit(EXIT_FAILURE);
    }

    /* set up interrupt trap */
    signal(SIGINT, INThandler);

    /* read through fastQ input sequence file */
    while (1)
    {
        in_buffer_count = 0;

        /* fill up the buffer */
        while (in_buffer_count < BUFFSIZE)
        {
            if (gzgets(in_fastq, in_buffer[in_buffer_count],
                       MAX_LINE_LENGTH) == Z_NULL)
                break;
            ++in_buffer_count;
        }

        /* dump the buffer to the output stream */
        for (i = 0; i < in_buffer_count; ++i)
        {
            if (i % 4 == 3)
            {
                j = 0;
                length = strlen(in_buffer[i]) - 1;
                if (p->flag & SCORE_ILLUMINA)
                {
                    /* only do Sanger to Illumina conversion */
                    while (j < length)
                    {
                        int score = in_buffer[i][j] + 31;
                        if (score > SCHAR_MAX)
                        {
                            fputs("\n\nError: the original Phred scores are "
                                    "not in standard Sanger format.\n\n",
                                  stderr);
                            exit(EXIT_FAILURE);
                        }
                        else
                            gzputc(out_fastq, score);
                        ++j;
                    }
                    gzputc(out_fastq, '\n');

                    if (p->flag & SCORE_ASCII)
                    {
                        /* do both numerical and Sanger to Illumina
                         * conversion here */
                        const char delim = ' ';
                        char *tok;
                        int score;
                        tok = strtok(in_buffer[i], &delim);
                        score = atoi(tok);
                        gzputc(out_fastq, score);
                        while (tok != NULL)
                        {
                            tok = strtok(NULL, &delim);
                            score = atoi(tok);
                            gzputc(out_fastq, score + 31);
                        }
                        gzputc(out_fastq, '\n');
                    }
                }
                else
                {
                    /* only do Illumina to Sanger conversion */
                    while (j < length)
                    {
                        int score;
                        score = in_buffer[i][j] - 31;
                        if ((score > SCHAR_MAX) || (score < 33))
                        {
                            fputs("\n\nError: the original Phred scores are "
                                    "not in Illumina format.\n\n", stderr);
                            exit(EXIT_FAILURE);
                        }
                        else
                            gzputc(out_fastq, in_buffer[i][j] - 31);
                        ++j;
                    }
                    gzputc(out_fastq, '\n');

                    if (p->flag & SCORE_ASCII)
                    {
                        /* do both numerical and Illumina to Sanger
                         * conversion here */
                        const char delim = ' ';
                        char *tok;
                        int score;
                        tok = strtok(in_buffer[i], &delim);
                        score = atoi(tok);
                        gzputc(out_fastq, score);
                        while (tok != NULL)
                        {
                            tok = strtok(NULL, &delim);
                            score = atoi(tok);
                            gzputc(out_fastq, score - 31);
                        }
                        gzputc(out_fastq, '\n');
                    }
                }
            }
            else
                gzputs(out_fastq, in_buffer[i]);
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
