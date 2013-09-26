/*************************************************************************
 *
 * File: pair.c
 *
 * Description: Functions to align reads in two paired fastq files
 *
 * Author: Daniel Garrigan
 *
 *************************************************************************/

#include "ngsutils.h"


/*
 *  Declare data structure to hold user options
 */

typedef struct _pair_params
{
	int flag;
	char seqFile1[FILENAME_MAX];
	char seqFile2[FILENAME_MAX];
	char outFile1[FILENAME_MAX];
	char outFile2[FILENAME_MAX];
} pair_p;


/*
 * Declare function prototypes
 */

int pair(int, char**);
pair_p* pair_read_params(int, char**);
int pair_usage(void);


/*
 * Entry point for the pair function
 */

int main_pair(int argc, char **argv)
{
	if (argv[0] == NULL)
		return pair_usage();
	else
		return pair(argc, argv);
}


/*
 * Main pair function
 */

int pair(int argc, char **argv)
{
	pair_p *p = NULL;
	gzFile seq1;
	gzFile seq2;
	gzFile out1;
	gzFile out2;

	/* Read and store user-supplied parameters */
	p = pair_read_params(argc, argv);

	/* Open sequence file 1 */
	if ((seq1 = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the first input fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open sequence file 2 */
	if ((seq2 = gzopen(p->seqFile2, "rb")) == NULL)
	{
			fputs("\n\nError: cannot open the second input fastq sequence file.\n\n", stderr);
			exit(EXIT_FAILURE);
	}

	/* Open the first fastq output stream */
	if ((out1 = gzopen(p->outFile1, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output stream for the first fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open the second fastq output stream */
	if ((out2 = gzopen(p->outFile2, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output stream for the second fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* TODO: Implement merging algorithm here */

	/* Set up interrupt trap */
	signal(SIGINT, INThandler);

	/* Close fastq 1 input stream */
	gzclose(seq1);

	/* Close fastq 2 input stream */
	gzclose(seq2);

	/* Close the output 1 stream */
	gzclose(out1);

	/* Close the output 1 stream */
	gzclose(out2);

	/* Take out the garbage */
	free(p);

	return 0;
}


/*
 * Read user-supplied command line parameters for the pair function
 */

pair_p* pair_read_params(int argc, char **argv)
{
	int c = 0;
	pair_p *p = NULL;

	/* Allocate memory for parameter data structure */
	p = (pair_p*) malloc(sizeof(pair_p));
	if (p == NULL)
	{
		fputs("\n\nError: memory allocation failure for pair user parameter data structure.\n", stderr);
		exit (EXIT_FAILURE);
	}

	/* Initialize some variables */
	opterr = 0;
	p->flag = 0;

   /* Read command line options */
	while ((c = getopt(argc, argv, "o:")) != -1)
	{
		switch(c)
		{
			case 'o':
				strcpy(p->outFile1, optarg);
				strcpy(p->outFile2, optarg);
				strcat(p->outFile1, ".1.gz");
				strcat(p->outFile2, ".2.gz");
				break;
			case '?':
				if (optopt == 'o')
					fprintf(stderr, "\n\nError: the option -%c requires an argument.\n\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "\n\nError: unknown option \"-%c\".\n\n", optopt);
				else
					fprintf(stderr, "\n\nError: unknown option character '\\x%x'.\n\n", optopt);
				exit(EXIT_FAILURE);
			default:
				pair_usage();
				exit(EXIT_FAILURE);
		}
	}

	/* Get the non-optioned arguments */
	/* First get the name of sequence input file 1 */
	if (argv[optind])
		strcpy(p->seqFile1, argv[optind]);
	else
	{
		fputs("\n\nError: need the input fastq sequence file name as a mandatory argument.\n\n", stderr);
		pair_usage();
		exit(EXIT_FAILURE);
	}

	/* Second get the name of sequence input file 2 */
	if (argv[optind + 1])
		strcpy(p->seqFile2, argv[optind + 1]);
	else
	{
		fputs("\n\nError: need the name of the second input fastq sequence file as a mandatory argument.\n\n", stderr);
		pair_usage();
		exit(EXIT_FAILURE);
	}

	return p;
}


/*
 * Prints a usage message for the pair function
 */

int pair_usage(void)
{
	fputs("\nUsage: NGSutils pair [options] <fastq_mate1> <fastq_mate2>\n\n", stderr);
	fputs("Options:        -o         prefix string for naming both output files\n\n", stderr);
	return 0;
}
