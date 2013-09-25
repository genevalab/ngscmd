/* File: pair.c
 * Description: Functions to align reads in two paired fastq files
 * Author: Daniel Garrigan
 */
#include "ngsutils.h"

int main_pair(int argc, char **argv)
{
	if (!argv[0])
		return pair_usage();
	else
		pair(argc, argv);
	return 0;
}

int pair(int argc, char **argv)
{
	return 0;
}


int pair_usage(void)
{
	fputs("\nUsage: ngsutils pair [options] <fastq_mate1> <fastq_mate2>\n\n", stderr);
	fputs("Options:        -o         prefix string for naming the output files\n", stderr);
	fputc(0x0a, stderr);
	return 0;
}