#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include <malloc.h>
#include <zlib.h>

/* Defined values */
#define NARGS 3
#define BUFFSIZE 2000
#define MAX_LINE_LENGTH 400

/* Function prototypes */
int main_fa2fq(int, char**);
int fa2fq(int, char**);
int main_pair(int, char**);
int pair(int, char**);
void INThandler(int);
int main_usage(void);
int fa2fq_usage(void);
int pair_usage(void);
