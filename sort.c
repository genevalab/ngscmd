/* File: sort.c
 * Description: Functions to perform lexical sort of reads by identifier string
 * Author: Daniel Garrigan
 */
#include "ngsutils.h"

/* Data structure to hold user options */
typedef struct _sort_params
{
	int flag;
	char seqFile[FILENAME_MAX];
	char outFile[FILENAME_MAX];
} sort_p;

/* Function prototypes */
int sort(int, char**);
sort_p* sort_read_params(int, char**);
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
	int i=0;
	char **seqLine;
	sort_p *p=NULL;
	gzFile seq;
	gzFile out;

	/* Read and store user-supplied parameters */
	p = sort_read_params(argc, argv);

	/* Open sequence input file */
	if ((seq = gzopen(p->seqFile, "rb")) == NULL)
	{
		fputs("Error opening the input fastq sequence file.\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open sequence output file */
	if ((out = gzopen(p->outFile, "wb")) == NULL)
	{
		fputs("Error opening the output fastq sequence file.\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Set up interrupt trap */
	signal(SIGINT, INThandler);

	/* Allocate memory for buffer */
	seqLine = (char**)malloc(BUFFSIZE*sizeof(char*));
	assert(seqLine);
	for (i=0; i<BUFFSIZE; ++i)
	{
		seqLine[i] = (char*)malloc(MAX_LINE_LENGTH*sizeof(char));
		assert(seqLine[i]);
	}

	/* Read through input sequence file */
	while (1)
	{
		/* Initialize counter for the number of lines in the buffer */
		int buffCount=0;

		/* Fill up the buffer */
		while (buffCount<BUFFSIZE)
		{
			/* Get line from sequence file */
			if (gzgets(seq, seqLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* Iterate the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* Tally scores along each position in the sequence */
		for (i=0; i<buffCount; ++i)
		{
			/* TODO: Add code to sort the sequences here */
		}

		/* If we are at the end of the file */
		if (buffCount<BUFFSIZE)
			break;
	}

	/* Close sequence input stream */
	gzclose(seq);

	/* Take out the garbage */
	for (i=0; i<BUFFSIZE; ++i)
		free(seqLine[i]);
	free(seqLine);
	free(p);

	return 0;
}

/* Read user-supplied command line parameters for the sort function */
sort_p* sort_read_params(int argc, char **argv)
{
	int c=0;
	sort_p *p=NULL;

	/* Allocate memory for parameter data structure */
	p = (sort_p*)malloc(sizeof(sort_p));
	if (p == NULL)
	{
		fputs("Error allocating memory for sort user parameter data structure.\n", stderr);
		exit (EXIT_FAILURE);
	}

	opterr=0;
	p->flag=0;

   /* Read command line options */
	while ((c = getopt(argc, argv, "o:")) != -1)
	{
		switch(c)
		{
			case 'o':
				strcpy(p->outFile, optarg);
				strcat(p->outFile, ".gz");
				break;
			case '?':
				if (optopt == 'o')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option: -%c.\n", optopt);
				else
					fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
				exit(EXIT_FAILURE);
			default:
				sort_usage();
				exit(EXIT_FAILURE);
		}
	}

	/* Get non-optioned arguments */
	if (argv[optind])
		strcpy(p->seqFile, argv[optind]);
	else
	{
		fputs("\nError: need input fastq sequence file name.\n", stderr);
		sort_usage();
		exit(EXIT_FAILURE);
	}
	
	return p;
}

/* Print usage message for the sort function */
int sort_usage(void)
{
	fputs("\nUsage: NGSutils sort [options] <fastq file>\n\n", stderr);
	fputs("Options:        -o         prefix string for name of fastq output file\n\n", stderr);
	return 0;
}