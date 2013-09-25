/* File: sort.c
 * Description: Functions to perform lexical sort of reads by identifier string
 * Author: Daniel Garrigan
 */
#include "ngsutils.h"

/* Function prototypes */
int sort(int, char**);
int sort_usage(void);

/* Entry point for the sort function */
int main_sort(int argc, char **argv)
{
	if (!argv[0])
		return sort_usage();
	else
		sort(argc, argv);
	return 0;
}

/* Main sort function */
int sort(int argc, char **argv)
{
	return 0;
}

/* Print usage message for the sort function */
int sort_usage(void)
{
	fputs("\nUsage: NGSutils sort [options] <fastq file>\n\n", stderr);
	fputs("Options:        -o         prefix string for name of fastq output file\n\n", stderr);
	return 0;
}