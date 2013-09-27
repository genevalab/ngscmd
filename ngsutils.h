/*************************************************************************
 *
 * File: ngsutils.c
 *
 * Description: Header for the NGSutils program
 *
 * Author: Daniel Garrigan
 *
 *************************************************************************/

#ifndef NGSUTILS_H
#define NGSUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <ctype.h>
#include <zlib.h>


/* Globally scoped definitions */
#define BUFFSIZE 2000
#define MAX_LINE_LENGTH 400
#define CONVERT_REV 0x1
#define CONVERT_NUM 0x2
#define CONVERT_ASCII 0x4

enum FUNC {FA2FQ, FQ2FA, PAIR, CONVERT, CLEAN, BYPOS, SORT, REVCOM, KMER};

#define STR_TRIM(s)                                                     \
{                                                                       \
	char *ptr = NULL;                                                   \
	for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);  \
	ptr[1] = '\0';                                                      \
}

#define STR_REVERSE(s)                                                  \
{                                                                       \
	char *p1 = NULL;                                                    \
	char *p2 = NULL;                                                    \
	for (p1=s, p2=s+strlen(s)-1; p2 > p1; ++p1, --p2)                   \
	{                                                                   \
		*p1 ^= *p2;                                                     \
		*p2 ^= *p1;                                                     \
		*p1 ^= *p2;                                                     \
	}                                                                   \
}


typedef struct _ngsParams
{
	int flag;
	int kmer_size;
	int func;
	char outFilePrefix[FILENAME_MAX - 5];
	char seqFile1[FILENAME_MAX];
	char seqFile2[FILENAME_MAX];
	char qualFile[FILENAME_MAX];
	char outFile1[FILENAME_MAX];
	char outFile2[FILENAME_MAX];
} ngsParams;


/* Function prototypes */
extern int ngs_fa2fq(ngsParams*);
extern int ngs_convert(ngsParams*);
extern int ngs_pair(ngsParams*);
extern int ngs_clean(ngsParams*);
extern int ngs_bypos(ngsParams*);
extern int ngs_sort(ngsParams*);
extern int ngs_revcom(ngsParams*);
extern int ngs_kmer(ngsParams*);
extern void INThandler(int);

#endif
