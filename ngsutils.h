#ifndef NGSUTILS_H
#define NGSUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <malloc.h>
#include <ctype.h>
#include <zlib.h>
#ifdef _MSC_VER
#include "getopt.h"
#else
#include <unistd.h>
#endif


#define BUFFSIZE 2000
#define MAX_LINE_LENGTH 400
#define FILE_NAME_LENGTH 200

/* Globally scoped variables */
extern char *optarg;
extern int optind, opterr, optopt;

/* Function prototypes */
int main_fa2fq(int, char**);
int fa2fq(int, char**);
int main_pair(int, char**);
int pair(int, char**);
int main_convert(int, char**);
int convert(int, char**);
void INThandler(int);
int main_usage(void);
int fa2fq_usage(void);
int pair_usage(void);
int convert_usage(void);
int getopt (int, char *const *, const char*);

#endif
