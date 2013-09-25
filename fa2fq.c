#include "ngsutils.h"

int main_fa2fq(int argc, char **argv)
{
	if (!argv[0])
		return fa2fq_usage();
	else
		fa2fq(argc, argv);
	return 0;
}

int fa2fq(int argc, char **argv)
{
	int i, c;
	char seqFile[FILE_NAME_LENGTH];
	char qualFile[FILE_NAME_LENGTH];
	char outFile[FILE_NAME_LENGTH];
	char **seqLine;
	char **qualLine;
	gzFile seq;
	gzFile qual;
	gzFile fastq_out;

   /* Read command line options */
	opterr = 0;
	while ((c = getopt(argc, argv, "o:")) != -1)
	{
		switch(c)
		{
			case 'o':
				strcpy(outFile, optarg);
				strcat(outFile, ".gz");
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
				fa2fq_usage();
				exit(EXIT_FAILURE);
		}
	}
	if (argv[optind])
		strcpy(seqFile, argv[optind]);
	else
	{
		fputs("\nError: need input fasta sequence file name.\n", stderr);
		fa2fq_usage();
		exit(EXIT_FAILURE);
	}
	if (argv[optind+1])
		strcpy(qualFile, argv[optind+1]);
	else
	{
		fputs("\nError: need the fasta quality file name.\n", stderr);
		fa2fq_usage();
		exit(EXIT_FAILURE);
	}

	/* Open sequence file */
	if ((seq = gzopen(seqFile, "rb")) == NULL)
	{
		fputs("Error opening the fasta sequence file.\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open the quality file */
	if ((qual = gzopen(qualFile, "rb")) == NULL)
	{
			fputs("Error opening the fasta quality file.\n", stderr);
			exit(EXIT_FAILURE);
	}

	/* Open fastq output stream */
	if ((fastq_out = gzopen(outFile, "wb")) == NULL)
	{
		fputs("Error opening the fastq output stream.\n", stderr);
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
		int buffCount = 0;

		/* Fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			if (gzgets(seq, seqLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
			{
				fputs("Error reading from fastq sequence file.\n", stderr);
				exit(EXIT_FAILURE);
			}

			if (gzgets(qual, qualLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
			{
				fputs("Error reading from fasta quality file.\n", stderr);
				exit(EXIT_FAILURE);
			}

			++buffCount;
		}

		/* Dump buffer to output stream */
		for (i=0; i < buffCount; ++i)
		{
			if (i%2)
			{
				gzputs(fastq_out, seqLine[i]);
				gzputc(fastq_out, 0x2b);
				gzputc(fastq_out, 0xa);
				gzputs(fastq_out, qualLine[i]);
			}
			else
			{
				gzputc(fastq_out, 0x40);
				gzputs(fastq_out, seqLine[i]+1);
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
	gzclose(fastq_out);

	/* Take out the garbage */
	for (i=0; i < BUFFSIZE; ++i)
	{
		free(seqLine[i]);
		free(qualLine[i]);
	}
	free(seqLine);
	free(qualLine);

	return 0;
}

int fa2fq_usage(void)
{
	fputc(0x0a, stderr);
	fputs("Usage: ngsutils fa2fq [options] <fasta file> <quality file>\n\n", stderr);
	fputs("Options:        -o         prefix string for name of fastq output file\n", stderr);
	fputc(0x0a, stderr);
	return 0;
}
