/* File: bypos.c
 * Description: Functions to calculate average quality score by base position in read
 * Author: Daniel Garrigan
 */

#include "ngsutils.h"

int main_bypos(int argc, char **argv)
{
	if (!argv[0])
		return bypos_usage();
	else
		bypos(argc, argv);
	return 0;
}

int bypos(int argc, char **argv)
{
	return 0;
}

int bypos_usage(void)
{
	fputs("\nUsage: ngsutils bypos <fastq file>\n\n", stderr);
	fputs("Note: bypos writes all output to STDOUT\n\n", stderr);
	return 0;
}
