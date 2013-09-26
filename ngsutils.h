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
#include <ctype.h>
#include <signal.h>
#include <limits.h>
#include <zlib.h>
#ifdef _MSC_VER
#include "getopt.h"
#else
#include <unistd.h>
#endif


/* Globally scoped definitions */
#define BUFFSIZE 2000
#define MAX_LINE_LENGTH 400


/* Globally scoped variables */
extern char *optarg;
extern int optind, opterr, optopt;


/* Function prototypes */
extern int main_fa2fq(int, char**);
extern int main_convert(int, char**);
extern int main_pair(int, char**);
extern int main_clean(int, char**);
extern int main_bypos(int, char**);
extern int main_sort(int, char**);
extern int main_revcom(int, char**);
extern int getopt (int, char *const *, const char*);
extern void INThandler(int);

#endif
