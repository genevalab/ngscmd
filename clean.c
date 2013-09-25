/* File: clean.c
 * Description: Functions to filter reads and bases according to quality
 * Author: Daniel Garrigan
 */

#include "ngsutils.h"

int main_clean(int argc, char **argv)
{
	if (!argv[0])
		return clean_usage();
	else
		clean(argc, argv);
	return 0;
}

int clean(int argc, char **argv)
{
	return 0;
}

int clean_usage(void)
{
	fputs("\nUsage: NGSutils clean [options] <fastq file>\n\n", stderr);
	fputs("Options:        -o         prefix string for name of fastq output file\n\n", stderr);
	return 0;
}
