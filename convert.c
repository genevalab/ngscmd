/* File: convert.c
 * Description: Functions to transform Phred scaled quality scores in fastq files
 * Author: Daniel Garrigan
 */
#include "ngsutils.h"

#define CONVERT_REV 0x1
#define CONVERT_NUM 0x2
#define CONVERT_ASCII 0x4

/* Data structure to hold user options */
typedef struct _convert_params
{
	int flag;
	char seqFile[FILENAME_MAX];
	char outFile[FILENAME_MAX];
} convert_p;

/* Function prototypes */
int convert(int, char**);
convert_p* convert_read_params(int, char**);
int convert_usage(void);

/* Entry point for convert function */
int main_convert(int argc, char **argv)
{
	if (!argv[0])
		return convert_usage();
	else
		convert(argc, argv);
	return 0;
}

/* Main convert function */
int convert(int argc, char **argv)
{
	int i;
	char **seqLine;
	convert_p *p;
	gzFile seq;
	gzFile out;

	/* Read and store user-supplied parameters */
	p = convert_read_params(argc, argv);

	/* Open sequence file */
	if ((seq = gzopen(p->seqFile, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open output fastq stream */
	if ((out = gzopen(p->outFile, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output fastq sequence file.\n\n", stderr);
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

	/* Read through fastq input sequence file */
	while (1)
	{
		int buffCount = 0;

		/* Fill up the buffer */
		while (buffCount<BUFFSIZE)
		{
			if (gzgets(seq, seqLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;
			++buffCount;
		}

		/* Dump buffer to output stream */
		for (i=0; i<buffCount; ++i)
		{
			if (i%4 == 3)
			{
				size_t j;
				if (p->flag & CONVERT_REV)
				{
					/* Only do Sanger to Illumina conversion */
					for (j=0; j<strlen(seqLine[i])-1; ++j)
					{
						int score = seqLine[i][j]+0x1f;
						if (score > SCHAR_MAX)
						{
							fputs("\n\nError: the original Phred scores are not in standard Sanger format.\n\n", stderr);
							exit(EXIT_FAILURE);
						}
						else
							gzputc(out, score);
					}
					gzputc(out, 0x0a);

					if (p->flag & CONVERT_NUM)
					{
						/* Do both numerical and Sanger to Illumina conversion here */
						const char delim = 0x20;
						char *tok;
						int score;
						tok = strtok(seqLine[i], &delim);
						score = atoi(tok);
						gzputc(out, score);
						while (tok != NULL)
						{
							tok = strtok(NULL, &delim);
							score = atoi(tok);
							gzputc(out, score+0x1f);
						}
						gzputc(out, 0x0a);
					}
				}
				else
				{
					/* Only do Illumina to Sanger conversion */
					for (j=0; j<strlen(seqLine[i])-1; ++j)
					{
						int score = seqLine[i][j]-0x1f;
						if ((score > SCHAR_MAX) || (score < 33))
						{
							fputs("\n\nError: the original Phred scores are not in Illumina format.\n\n", stderr);
							exit(EXIT_FAILURE);
						}
						else
							gzputc(out, seqLine[i][j] - 0x1f);
					}
					gzputc(out, 0x0a);

					if (p->flag & CONVERT_NUM)
					{
						/* Do both numerical and Illumina to Sanger conversion here */
						const char delim = 0x20;
						char *tok;
						int score;
						tok = strtok(seqLine[i], &delim);
						score = atoi(tok);
						gzputc(out, score);
						while (tok != NULL)
						{
							tok = strtok(NULL, &delim);
							score = atoi(tok);
							gzputc(out, score - 0x1f);
						}
						gzputc(out, 0x0a);
					}
				}
			}
			else
				gzputs(out, seqLine[i]);
		}

		/* If we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* Close fastq input sequence file stream */
	gzclose(seq);

	/* Close fastq output sequence file stream */
	gzclose(out);

	/* Take out the garbage */
	for (i=0; i<BUFFSIZE; ++i)
		free(seqLine[i]);
	free(seqLine);
	free(p);

	return 0;
}

/* Read user-supplied command line parameters for the convert function */
convert_p* convert_read_params(int argc, char **argv)
{
	int c;
	convert_p *p;

	/* Allocate memory for parameter data structure */
	p = (convert_p*)malloc(sizeof(convert_p));
	if (p == NULL)
	{
		fputs("\n\nError: memory allocation failure for convert user parameter data structure.\n\n", stderr);
		exit (EXIT_FAILURE);
	}

	opterr = 0;
	p->flag = 0;

   /* Read command line options */
	while ((c = getopt(argc, argv, "snao:")) != -1)
	{
		switch(c)
		{
			case 'o':
				strcpy(p->outFile, optarg);
				strcat(p->outFile, ".gz");
				break;
			case 'a':
				p->flag |= CONVERT_NUM;
				break;
			case 'n':
				p->flag |= CONVERT_ASCII;
				break;
			case 's':
				p->flag |= CONVERT_REV;
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
				convert_usage();
				exit(EXIT_FAILURE);
		}
	}

	/* Get non-optioned arguments */
	if (argv[optind])
		strcpy(p->seqFile, argv[optind]);
	else
	{
		fputs("\n\nError: need the input fastq sequence file name as mandatory argument.\n", stderr);
		convert_usage();
		exit(EXIT_FAILURE);
	}
	
	return p;
}

/* Print usage message for the convert function */
int convert_usage(void)
{
	fputs("\nUsage: NGSutils convert [options] <fastq file>\n\n", stderr);
	fputs("Options:        -o         prefix string for name of fastq output file\n", stderr);
	fputs("                -s         convert from 33-126 scale to 64-126 scale\n", stderr);
	fputs("                           default: 64-126 to 33-126 scale\n", stderr);
	fputs("                -a         convert from numerical scores to ASCII\n", stderr);
	fputs("                -n         convert from ASCII scores to numerical\n", stderr);
	fputc(0x0a, stderr);
	return 0;
}
