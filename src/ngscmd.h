/* ngscmd - A collection of utilites for processing next-generation sequence files
   Copyright (C) 2014 Laboratory for Comparative Population Genomics

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Daniel Garrigan, dgarriga@lcpg.org */

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
extern "C"
{
#endif

/* Define constants */
#ifndef MAX_LINE_LENGTH
#define MAX_LINE_LENGTH 400
#endif
#define BUFFSIZE 2000
#define TWO_INPUTS 0x1
#define SCORE_NUM 0x8
#define SCORE_ASCII 0x10
#define SCORE_ILLUMINA 0x20

  typedef struct _ngs_params
  {
    int flag;
    int func;
    int num_ambig;
    int min_read_length;
    int trim_quality;
    int kmer_size;
    int gap_size;
    char outfile_prefix[FILENAME_MAX-5];
    char seqfile_name1[FILENAME_MAX];
    char seqfile_name2[FILENAME_MAX];
    char outfile_name1[FILENAME_MAX];
    char outfile_name2[FILENAME_MAX];
  } ngs_params;


/* Inline functions */

/* Remove the trailing newline character from a string */
#define chomp(s)                          \
{                                         \
    char *ptr = s + strlen(s) - 1;        \
    while ((ptr >= s) && isspace(*ptr))   \
        --ptr;                            \
    ptr[1] = '\0';                        \
}

/* Reverse a string */
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



/* Function prototypes */
  extern int ngs_filter (ngs_params *);
  extern int ngs_trim (ngs_params *);
  extern int ngs_pair (ngs_params *);
  extern int ngs_score (ngs_params *);
  extern int ngs_rmdup (ngs_params *);
  extern int ngs_kmer (ngs_params *);
  extern int ngs_score (ngs_params *);
  extern int ngs_join (ngs_params *);
  extern void INThandler (int);

#ifdef __cplusplus
}
#endif

#endif          /* NGSUTILS_H */
