/*  ngsutils.h -- interface of the 'ngslib' library for handling next-generation sequences in fastQ format
	version 0.1, September 27, 2013

	Copyright (c) 2013 Daniel Garrigan

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

/* constants */
#define BUFFSIZE 2000
#define MAX_LINE_LENGTH 400
#define CONVERT_REV 0x1
#define CONVERT_NUM 0x2
#define CONVERT_ASCII 0x4
enum FUNC {FA2FQ, FQ2FA, PAIR, CONVERT, CLEAN, BYPOS, SORT, REVCOM, KMER};

/* inline functions */
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

/* function prototypes */
extern int  ngs_fa2fq(ngsParams*);
extern int  ngs_convert(ngsParams*);
extern int  ngs_pair(ngsParams*);
extern int  ngs_clean(ngsParams*);
extern int  ngs_bypos(ngsParams*);
extern int  ngs_sort(ngsParams*);
extern int  ngs_revcom(ngsParams*);
extern int  ngs_kmer(ngsParams*);
extern void INThandler(int);

#ifdef __cplusplus
}
#endif

#endif /* NGSUTILS_H */
