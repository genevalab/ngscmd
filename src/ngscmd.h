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

#ifndef NGSUTILS_H
#define NGSUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <ctype.h>
#include <zlib.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* constants */
#ifndef MAX_LINE_LENGTH
#define MAX_LINE_LENGTH 400
#endif
#define BUFFSIZE 2000
#define TWO_INPUTS 0x1
#define SCORE_NUM 0x8
#define SCORE_ASCII 0x10
#define SCORE_ILLUMINA 0x20

typedef struct _ngsParams
{
    int flag;
    int func;
    int num_ambig;
    int min_read_length;
    int trim_quality;
    int kmer_size;
    char outFilePrefix[FILENAME_MAX - 5];
    char seqFile1[FILENAME_MAX];
    char seqFile2[FILENAME_MAX];
    char outFile1[FILENAME_MAX];
    char outFile2[FILENAME_MAX];
} ngsParams;


/* inline functions */

/* remove the trailing newline character from a string */
#define chomp(s)                          \
{                                         \
    char *ptr = s + strlen(s) - 1;        \
    while ((ptr >= s) && isspace(*ptr))   \
        --ptr;                            \
    ptr[1] = '\0';                        \
}

/* reverse a string */
#define strrev(s)                         \
{                                         \
    char *p1 = s;                         \
    char *p2 = s + strlen(s) - 1;         \
    while (p2 > p1)                       \
    {                                     \
        *p1 ^= *p2;                       \
        *p2 ^= *p1;                       \
        *p1 ^= *p2;                       \
        ++p1;                             \
        --p2;                             \
    }                                     \
}

/* function prototypes */
extern int ngs_filter(ngsParams*);
extern int ngs_trim(ngsParams*);
extern int ngs_pair(ngsParams*);
extern int ngs_score(ngsParams*);
extern int ngs_rmdup(ngsParams*);
extern int ngs_kmer(ngsParams*);
extern void INThandler(int);

#ifdef __cplusplus
}
#endif

#endif /* NGSUTILS_H */
