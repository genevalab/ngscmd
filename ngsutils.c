#include "ngsutils.h"

#define VERSION 0.1

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
	else
	{
		fprintf(stderr, "Error: %s function is unrecognized\n", argv[1]);
		main_usage();
		return 1;
	}
}

void INThandler(int sig)
{
	signal(sig, SIG_IGN);
	fputs("\nCtl-C caught, exiting program\n", stderr);
	exit(EXIT_SUCCESS);
}

int main_usage(void)
{
	fputc(0x0a, stderr);
	fputs("Usage: ngsutils [function] [options] <infile> ...\n\n", stderr);
	fputs("Functions:       fa2fq      merge fasta and quality file to fastq format\n", stderr);
	fputs("                 pair       aligned mated pairs in two fastq files\n", stderr);
	fputs("                 convert    convert quality scores\n", stderr);
	fputc(0x0a, stderr);
	return 0;
}
