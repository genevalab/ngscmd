/* File: ngsutils.c
 * Description: Main entry point for the NGSutils program
 * Author: Daniel Garrigan
 */
#include "ngsutils.h"

/* Definitions for the main function */
#define VERSION 0.1


/* Function prototypes */
int main_usage(void);


/* Entry point for the NGSutils program */
int main(int argc, char **argv)
{
	if (argc < 2)
		return main_usage();
	if (strcmp(argv[1], "fa2fq") == 0)
		return main_fa2fq(argc-1, argv+1);
	else if (strcmp(argv[1], "pair") == 0)
		return main_pair(argc-1, argv+1);
	else if (strcmp(argv[1], "convert") == 0)
		return main_convert(argc-1, argv+1);
	else if (strcmp(argv[1], "clean") == 0)
		return main_clean(argc-1, argv+1);
	else if (strcmp(argv[1], "bypos") == 0)
		return main_bypos(argc-1, argv+1);
	else if (strcmp(argv[1], "sort") == 0)
		return main_sort(argc-1, argv+1);
	else
	{
		fprintf(stderr, "\n\nError: the function \"%s\" is not recognized\n", argv[1]);
		main_usage();
		return 1;
	}
}


/* Handle an interrupt signal */
void INThandler(int sig)
{
	signal(sig, SIG_IGN);
	fputs("\nCtl-C caught... exiting program\n", stderr);
	exit(EXIT_SUCCESS);
}


/* Print usage message for the NGSutils program */
int main_usage(void)
{
	fputs("\nUsage: NGSutils <function> [options] <infile> ...\n\n", stderr);
	fputs("Functions:       fa2fq      convert between fasta/quality files and fastq format\n", stderr);
	fputs("                 pair       aligned mated pairs in two fastq files\n", stderr);
	fputs("                 convert    convert Phred scaled quality scores\n", stderr);
	fputs("                 clean      perform a variety of cleaning procedures for reads\n", stderr);
	fputs("                 bypos      show average quality by sequence position\n", stderr);
	fputs("                 sort       lexical sort of reads by identifier string\n\n", stderr);
	return 0;
}
