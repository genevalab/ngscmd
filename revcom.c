/*************************************************************************
 *
 * File: revcom.c
 *
 * Description: Functions to reverse complement all sequences in a 
 *              fastq file
 *
 * Author: Daniel Garrigan
 *
 *************************************************************************/

#include "ngsutils.h"


/*
 *  Declare the revcom_p data structure to hold user options
 */

typedef struct _revcom_params
{
	int flag;
	char seqFile[FILENAME_MAX];
	char outFile[FILENAME_MAX];
} revcom_p;


/*
 * Declare function prototypes
 */

int revcom(int, char**);
revcom_p* revcom_read_params(int, char**);
char *trim(char*);
char *reverse(char*);
int revcom_usage(void);


/*
 * Entry point for the revcom function
 */

int main_revcom(int argc, char **argv)
{
	if (argv[0] == NULL)
		return revcom_usage();
	else
		return revcom(argc, argv);
}


/*
 * Reverse complements a sequence in fastq format
 */

int revcom(int argc, char **argv)
{
	int i = 0;
	char **seqLine;
	revcom_p *p = NULL;
	gzFile seq;
	gzFile out;

	/* Read and store user-supplied parameters */
	p = revcom_read_params(argc, argv);

	/* Open sequence input file */
	if ((seq = gzopen(p->seqFile, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open sequence output file */
	if ((out = gzopen(p->outFile, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Set up interrupt trap */
	signal(SIGINT, INThandler);

	/* Allocate memory for buffer */
	seqLine = (char**) malloc(BUFFSIZE * sizeof(char*));
	if (seqLine == NULL)
	{
		fputs("Memory allocation failure for seqLine.1\n", stderr);
		exit (EXIT_FAILURE);
	}
	for (i = 0; i < BUFFSIZE; ++i)
	{
		seqLine[i] = (char*) malloc(MAX_LINE_LENGTH * sizeof(char));
		if (seqLine[i] == NULL)
		{
			fputs("Memory allocation failure for seqLine.2\n", stderr);
			exit (EXIT_FAILURE);
		}
	}

	/* Read through input sequence file */
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

			/* Iterate the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* Reverse complement bases and reverse quality scores */
		for (i = 0; i < buffCount; ++i)
		{
			int j = i % 4;
			if ((j == 1) || (j == 3))
			{
				char *rev;
				rev = trim(seqLine[i]);
				rev = reverse(rev);
				if (j == 1)
				{
					size_t k;
					for (k = 0; k < strlen(rev); ++k)
					{
						if (rev[k] == 'A')
							gzputc(out, 'T');
						else if (rev[k] == 'C')
							gzputc(out, 'G');
						else if (rev[k] == 'G')
							gzputc(out, 'C');
						else if (rev[k] == 'T')
							gzputc(out, 'A');
						else
							gzputc(out, rev[k]);
					}
				}
				else
					gzputs(out, rev);
				gzputc(out, '\n');
			}
			else
				gzputs(out, seqLine[i]);
		}

		/* If we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* Close sequence input stream */
	gzclose(seq);

	/* Close sequence output stream */
	gzclose(out);

	/* Take out the garbage */
	for (i = 0; i < BUFFSIZE; ++i)
		free(seqLine[i]);
	free(seqLine);
	free(p);

	return 0;
}


/*
 * Trims the last character from a string
 */

char *trim(char *s)
{
	char *ptr = NULL;

	if (!s)
		return NULL;
	if (!*s)
		return s;
	for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
	ptr[1] = '\0';
	return s;
}


/*
 * Reverses a string
 */

char *reverse(char *s)
{
	char *p1 = NULL;
	char *p2 = NULL;

	if (! s || ! *s)
		return s;
	for (p1=s, p2=s+strlen(s)-1; p2 > p1; ++p1, --p2)
	{
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}

	return s;
}


/*
 * Read user-supplied command line parameters for the revcom function
 */

revcom_p* revcom_read_params(int argc, char **argv)
{
	int c = 0;
	revcom_p *p = NULL;
	
	/* Allocate memory for parameter data structure */
	p = (revcom_p*) malloc(sizeof(revcom_p));
	if (p == NULL)
	{
		fputs("\n\nError: memory allocation failure for revcom user parameter data structure.\n\n", stderr);
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
				exit (EXIT_FAILURE);
			default:
				revcom_usage();
				exit (EXIT_FAILURE);
		}
	}

	/* Get the non-optioned arguments */
	/* Get the name of the sequence input file */
	if (argv[optind])
		strcpy(p->seqFile, argv[optind]);
	else
	{
		fputs("\n\nError: need the input fastq sequence file name as a mandatory argument.\n", stderr);
		revcom_usage();
		exit (EXIT_FAILURE);
	}
	
	return p;
}


/*
 * Prints a usage message for the revcom function
 */

int revcom_usage(void)
{
	fputs("\nUsage: NGSutils revcom [options] <fastq file>\n\n", stderr);
	fputs("Options:        -o         prefix string for name of fastq output file\n\n", stderr);
	return 0;
}
