/*************************************************************************
 *
 * File: clean.c
 *
 * Description: Functions to filter reads and bases according to quality
 *
 * Author: Daniel Garrigan
 *
 *************************************************************************/

#include "ngsutils.h"


/***************************************************************************
 *
 *  Declare data structure to hold user options
 *
 ***************************************************************************/

typedef struct _clean_params
{
	int flag;
	char seqFile[FILENAME_MAX];
	char outFile[FILENAME_MAX];
} clean_p;


/***************************************************************************
 *
 * Declare function prototypes
 *
 **************************************************************************/

int clean(int, char**);

clean_p* clean_read_params(int, char**);

int clean_usage(void);


/***************************************************************************
 * Function: main_clean()
 *
 * Description: entry point for the clean function
 ***************************************************************************/

int main_clean(int argc, char **argv)
{
	if (!argv[0])
		return clean_usage();
	else
		return clean(argc, argv);
}


/***************************************************************************
 * Function: clean()
 *
 * Description: main clean function
 ***************************************************************************/

int clean(int argc, char **argv)
{
	int i=0;
	char **seqLine;
	clean_p *p=NULL;
	gzFile seq;
	gzFile out;

	/* Read and store user-supplied parameters */
	p = clean_read_params(argc, argv);

	/* Open sequence input file */
	if ((seq = gzopen(p->seqFile, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open sequence output file */
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
			/* TODO: Add code to filter and trim sequences here */
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


/***************************************************************************
 * Function: clean_read_params()
 *
 * Description: read user-supplied command line parameters for the clean 
 *              function
 ***************************************************************************/

clean_p* clean_read_params(int argc, char **argv)
{
	int c=0;
	clean_p *p=NULL;

	/* Allocate memory for parameter data structure */
	p = (clean_p*)malloc(sizeof(clean_p));
	if (p == NULL)
	{
		fputs("\n\nError: memory allocation failure for clean user parameter data structure.\n\n", stderr);
		exit (EXIT_FAILURE);
	}

	/* Initialize some variables */
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
					fprintf(stderr, "\n\nError: the option -%c requires an argument.\n\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "\n\nError: unknown option \"-%c\".\n\n", optopt);
				else
					fprintf(stderr, "\n\nError: unknown option character '\\x%x'.\n\n", optopt);
				exit(EXIT_FAILURE);
			default:
				clean_usage();
				exit(EXIT_FAILURE);
		}
	}

	/* Get non-optioned arguments */
	/* Get the name of the input sequence file */
	if (argv[optind])
		strcpy(p->seqFile, argv[optind]);
	else
	{
		fputs("\n\nError: need the input fastq sequence file name as mandatory argument.\n", stderr);
		clean_usage();
		exit(EXIT_FAILURE);
	}
	
	return p;
}


/***************************************************************************
 * Function: clean_usage()
 *
 * Description: prints a usage message for the clean function
 ***************************************************************************/

int clean_usage(void)
{
	fputs("\nUsage: NGSutils clean [options] <fastq file>\n\n", stderr);
	fputs("Options:        -o         prefix string for name of fastq output file\n\n", stderr);
	return 0;
}
