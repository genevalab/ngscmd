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

/* Function prototypes */
int main_fa2fq(int, char**);
int fa2fq(int, char**);
int main_pair(int, char**);
int pair(int, char**);
void INThandler(int);
int main_usage(void);
int fa2fq_usage(void);
int pair_usage(void);

#endif
