/* File: bypos.c
 * Description: Functions to calculate average quality score by base position in read
 * Author: Daniel Garrigan
 */
#include "ngsutils.h"

/* Data structure to hold user options */
typedef struct _bypos_params
{
	int flag;
	char seqFile[FILENAME_MAX];
} bypos_p;

/* Function prototypes */
int bypos(int, char**);
bypos_p* bypos_read_params(int, char**);
int bypos_usage(void);

/* Entry point for the bypos function */
int main_bypos(int argc, char **argv)
{
	if (!argv[0])
		return bypos_usage();
	else
		bypos(argc, argv);
	return 0;
}

/* Main bypos function */
int bypos(int argc, char **argv)
{
	int i=0;
	int max_pos=0;
	unsigned long long int *score_sum=NULL;
	unsigned long long int *num_bases=NULL;
	char **seqLine;
	bypos_p *p=NULL;
	gzFile seq;

	/* Read and store user-supplied parameters */
	p = bypos_read_params(argc, argv);

	/* Open sequence file */
	if ((seq = gzopen(p->seqFile, "rb")) == NULL)
	{
		fputs("Error opening the input fastq sequence file.\n", stderr);
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

	/* Allocate memory for score_sum array */
	score_sum = (unsigned long long int*)calloc(MAX_LINE_LENGTH, sizeof(unsigned long long int));
	assert(score_sum);

	/* Allocate memory for num_bases array */
	num_bases = (unsigned long long int*)calloc(MAX_LINE_LENGTH, sizeof(unsigned long long int));
	assert(num_bases);

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
			if (i%4 == 3)
			{
				size_t j;
				int k = (int)(strlen(seqLine[i]) - 1);
				max_pos = k > max_pos ? k : max_pos;
				for (j=0; j<strlen(seqLine[i])-1; ++j)
				{
					score_sum[j] += (unsigned long long int)(seqLine[i][j]-33);
					++num_bases[j];
				}
			}
		}

		/* If we are at the end of the file */
		if (buffCount<BUFFSIZE)
			break;
	}

	/* Close sequence input stream */
	gzclose(seq);

	/* Print results to STDOUT */
	puts("Position  \tNo. Bases   \tAvg. Score");
	for (i=0; i<max_pos; ++i)
		if (num_bases[i]>0)
			printf("%-10d\t%-12llu\t%-12.7lf\n", i+1, num_bases[i], (double)score_sum[i]/num_bases[i]);

	/* Take out the garbage */
	for (i=0; i<BUFFSIZE; ++i)
		free(seqLine[i]);
	free(seqLine);
	free(num_bases);
	free(score_sum);
	free(p);

	return 0;
}

/* Read user-supplied command line parameters for the bypos function */
bypos_p* bypos_read_params(int argc, char **argv)
{
	int c=0;
	bypos_p *p=NULL;

	/* Allocate memory for parameter data structure */
	p = (bypos_p*)malloc(sizeof(bypos_p));
	if (p == NULL)
	{
		fputs("Error allocating memory for bypos user parameter data structure.\n", stderr);
		exit (EXIT_FAILURE);
	}

	opterr=0;
	p->flag=0;

   /* Read command line options */
	while ((c = getopt(argc, argv, "h")) != -1)
	{
		switch(c)
		{
			case 'h':
				bypos_usage();
				exit(EXIT_SUCCESS);
			case '?':
				if (optopt == 'o')
					fprintf(stderr, "Option -%c requires an argument.\n", optopt);
				else if (isprint(optopt))
					fprintf(stderr, "Unknown option: -%c.\n", optopt);
				else
					fprintf(stderr, "Unknown option character '\\x%x'.\n", optopt);
				exit(EXIT_FAILURE);
			default:
				bypos_usage();
				exit(EXIT_FAILURE);
		}
	}

	/* Get non-optioned arguments */
	if (argv[optind])
		strcpy(p->seqFile, argv[optind]);
	else
	{
		fputs("\nError: need input fastq sequence file name.\n", stderr);
		bypos_usage();
		exit(EXIT_FAILURE);
	}
	
	return p;
}

/* Print usage message for the bypos function */
int bypos_usage(void)
{
	fputs("\nUsage: NGSutils bypos <fastq file>\n\n", stderr);
	fputs("Note: bypos writes all output to STDOUT\n\n", stderr);
	return 0;
}
