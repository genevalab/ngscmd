/* File: sort.c
 * Description: Functions to perform lexical sort of reads by identifier string
 * Author: Daniel Garrigan
 */

#include "ngsutils.h"

int main_sort(int argc, char **argv)
{
	if (!argv[0])
		return sort_usage();
	else
		sort(argc, argv);
	return 0;
}

int sort(int argc, char **argv)
{
	return 0;
}

int sort_usage(void)
{
	fputs("\nUsage: NGSutils sort [options] <fastq file>\n\n", stderr);
	fputs("Options:        -o         prefix string for name of fastq output file\n\n", stderr);
	return 0;
}