/* File: fa2sq.c
 * Description: Functions convert between fasta/quality files and fastq file
 * Author: Daniel Garrigan
 */
#include "ngsutils.h"

#define FATOFQ_REV 0x1

/* Data structure to hold user options */
typedef struct _fa2fq_params
{
	int flag;
	char seqFile[FILENAME_MAX];
	char qualFile[FILENAME_MAX];
	char outFile[FILENAME_MAX];
} fa2fq_p;

/* Function prototypes */
int fa2fq(int, char**);
fa2fq_p* fa2fq_read_params(int, char**);
int fa2fq_usage(void);

/* Entry point for fa2fq function */
int main_fa2fq(int argc, char **argv)
{
	if (!argv[0])
		return fa2fq_usage();
	else
		fa2fq(argc, argv);
	return 0;
}

/* Main fa2fq function */
int fa2fq(int argc, char **argv)
{
	int i;
	char **seqLine;
	char **qualLine;
	fa2fq_p *p;
	gzFile seq;
	gzFile qual;
	gzFile out;

	/* Read and store user-supplied parameters */
	p = fa2fq_read_params(argc, argv);

	/* Open sequence file */
	if ((seq = gzopen(p->seqFile, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fasta sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open the quality file */
	if ((qual = gzopen(p->qualFile, "rb")) == NULL)
	{
			fputs("\n\nError: cannot open the input fasta quality file.\n\n", stderr);
			exit(EXIT_FAILURE);
	}

	/* Open fastq output stream */
	if ((out = gzopen(p->outFile, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output fastq sequence file.\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Set up interrupt trap */
	signal(SIGINT, INThandler);

	/* Allocate memory for buffer */
	seqLine = (char**)malloc(BUFFSIZE*sizeof(char*));
	assert(seqLine);
	qualLine = (char**)malloc(BUFFSIZE*sizeof(char*));
	assert(qualLine);
	for (i=0; i < BUFFSIZE; ++i)
	{
		seqLine[i] = (char*)malloc(MAX_LINE_LENGTH*sizeof(char));
		assert(seqLine[i]);
		qualLine[i] = (char*)malloc(MAX_LINE_LENGTH*sizeof(char));
		assert(qualLine[i]);
	}

	/* Read through both files */
	while (1)
	{
		/* Initialize counter for the number of lines in the buffer */
		int buffCount = 0;

		/* Fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			/* Get line from sequence file */
			if (gzgets(seq, seqLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* Get line from quality file */
			if (gzgets(qual, qualLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* Iterate the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* Dump buffer to output stream */
		for (i=0; i < buffCount; ++i)
		{
			if (i%2)
			{
				gzputs(out, seqLine[i]);
				gzputc(out, 0x2b);
				gzputc(out, 0xa);
				gzputs(out, qualLine[i]);
			}
			else
			{
				gzputc(out, 0x40);
				gzputs(out, seqLine[i]+1);
			}
		}

		/* If we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* Close sequence input stream */
	gzclose(seq);

	/* Close quality input stream */
	gzclose(qual);

	/* Close the output stream */
	gzclose(out);

	/* Take out the garbage */
	for (i=0; i < BUFFSIZE; ++i)
	{
		free(seqLine[i]);
		free(qualLine[i]);
	}
	free(seqLine);
	free(qualLine);
	free(p);

	return 0;
}

/* Read user-supplied command line parameters for the fa2fq function */
fa2fq_p* fa2fq_read_params(int argc, char **argv)
{
	int c;
	fa2fq_p *p;

	/* Allocate memory for parameter data structure */
	p = (fa2fq_p*)malloc(sizeof(fa2fq_p));
	if (p == NULL)
	{
		fputs("\n\nError: memory allocation failure for fa2fq user parameter data structure.\n\n", stderr);
		exit (EXIT_FAILURE);
	}

	opterr = 0;
	p->flag = 0;

   /* Read command line options */
	while ((c = getopt(argc, argv, "ro:")) != -1)
	{
		switch(c)
		{
			case 'o':
				strcpy(p->outFile, optarg);
				strcat(p->outFile, ".gz");
				break;
			case 'r':
				p->flag |= FATOFQ_REV;
				break;
			case '?':
				if (optopt == 'o')
					fprintf(stderr, "\n\nError: the option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "\n\nError: unknown option \"-%c\".\n", optopt);
				else
					fprintf(stderr, "\n\nError: unknown option character '\\x%x'.\n", optopt);
				exit(EXIT_FAILURE);
			default:
				fa2fq_usage();
				exit(EXIT_FAILURE);
		}
	}
	if (argv[optind])
		strcpy(p->seqFile, argv[optind]);
	else
	{
		fputs("\nError: need the input fasta sequence file name as a mandatory argument.\n", stderr);
		fa2fq_usage();
		exit(EXIT_FAILURE);
	}
	if (argv[optind+1])
		strcpy(p->qualFile, argv[optind+1]);
	else
	{
		fputs("\nError: need the fasta quality file name as a mandatory argument.\n", stderr);
		fa2fq_usage();
		exit(EXIT_FAILURE);
	}

	return p;
}

/* Print usage message for the fa2fq function */
int fa2fq_usage(void)
{
	fputs("\nUsage: NGSutils fa2fq [options] <fasta/q input file> [quality file]\n\n", stderr);
	fputs("Options:        -o         prefix string for name of fastq/fasta/quality output files\n", stderr);
	fputs("                -r         split fastq file into separate fasta and quality files\n", stderr);
	fputc(0x0a, stderr);
	return 0;
}
