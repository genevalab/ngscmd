/* File: convert.c
 * Description: Functions to transform Phred scaled quality scores in fastq files
 * Author: Daniel Garrigan
 */
#include "ngsutils.h"

#define CONVERT_REV 0x1
#define CONVERT_NUM 0x2
#define CONVERT_ASCII 0x4

int main_convert(int argc, char **argv)
{
	if (!argv[0])
		return convert_usage();
	else
		convert(argc, argv);
	return 0;
}

int convert(int argc, char **argv)
{
	int i, c;
	int convert_flag = 0;
	char seqFile[FILE_NAME_LENGTH];
	char outFile[FILE_NAME_LENGTH];
	char **seqLine;
	gzFile fastq_in;
	gzFile fastq_out;

   /* Read command line options */
	opterr = 0;
	while ((c = getopt(argc, argv, "snao:")) != -1)
	{
		switch(c)
		{
			case 'o':
				strcpy(outFile, optarg);
				strcat(outFile, ".gz");
				break;
			case 'a':
				convert_flag |= CONVERT_NUM;
				break;
			case 'n':
				convert_flag |= CONVERT_ASCII;
				break;
			case 's':
				convert_flag |= CONVERT_REV;
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
				convert_usage();
				exit(EXIT_FAILURE);
		}
	}
	if (argv[optind])
		strcpy(seqFile, argv[optind]);
	else
	{
		fputs("\nError: need input fastq sequence file name.\n", stderr);
		convert_usage();
		exit(EXIT_FAILURE);
	}

	/* Open sequence file */
	if ((fastq_in = gzopen(seqFile, "rb")) == NULL)
	{
		fputs("Error opening the fastq sequence file.\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open output fastq stream */
	if ((fastq_out = gzopen(outFile, "wb")) == NULL)
	{
		fputs("Error opening the output stream.\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Set up interrupt trap */
	signal(SIGINT, INThandler);

	/* Allocate memory for buffer */
	seqLine = (char**)malloc(BUFFSIZE*sizeof(char*));
	assert(seqLine);
	for (i=0; i < BUFFSIZE; ++i)
	{
		seqLine[i] = (char*)malloc(MAX_LINE_LENGTH*sizeof(char));
		assert(seqLine[i]);
	}

	/* Read through fastq input sequence file */
	while (1)
	{
		int buffCount = 0;

		/* Fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			if (gzgets(fastq_in, seqLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;
			++buffCount;
		}

		/* Dump buffer to output stream */
		for (i=0; i < buffCount; ++i)
		{
			if (i%4 == 3)
			{
				size_t j;
				if (convert_flag & CONVERT_REV)
				{
					/* Only do Sanger to Illumina conversion */
					for (j=0; j < strlen(seqLine[i])-1; ++j)
						gzputc(fastq_out, seqLine[i][j] + 0x1f);
					gzputc(fastq_out, 0x0a);
					if (convert_flag & CONVERT_NUM)
					{
						/* Do both numerical and Sanger to Illumina conversion here */
						const char delim = 0x20;
						char *tok;
						int score;
						tok = strtok(seqLine[i], &delim);
						score = atoi(tok);
						gzputc(fastq_out, score);
						while(tok != NULL)
						{
							tok = strtok(NULL, &delim);
							score = atoi(tok);
							gzputc(fastq_out, score + 0x1f);
						}
						gzputc(fastq_out, 0x0a);
					}
				}
				else
				{
					/* Only do Illumina to Sanger conversion */
					for (j=0; j < strlen(seqLine[i])-1; ++j)
						gzputc(fastq_out, seqLine[i][j] - 0x1f);
					gzputc(fastq_out, 0x0a);
					if (convert_flag & CONVERT_NUM)
					{
						/* Do both numerical and Illumina to Sanger conversion here */
						const char delim = 0x20;
						char *tok;
						int score;
						tok = strtok(seqLine[i], &delim);
						score = atoi(tok);
						gzputc(fastq_out, score);
						while(tok != NULL)
						{
							tok = strtok(NULL, &delim);
							score = atoi(tok);
							gzputc(fastq_out, score - 0x1f);
						}
						gzputc(fastq_out, 0x0a);
					}
				}
			}
			else
				gzputs(fastq_out, seqLine[i]);
		}

		/* If we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* Close fastq input sequence file stream */
	gzclose(fastq_in);

	/* Close fastq output sequence file stream */
	gzclose(fastq_out);

	/* Take out the garbage */
	for (i=0; i < BUFFSIZE; ++i)
		free(seqLine[i]);
	free(seqLine);

	return 0;
}

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
