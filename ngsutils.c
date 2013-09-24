#include "ngsutils.h"

int main(int argc, char **argv)
{
	if (argc < 2)
		return main_usage();
	if (strcmp(argv[1], "fa2fq") == 0)
		return fa2fq(argc-2, argv+2);
	else if (strcmp(argv[1], "pair") == 0)
		return pair(argc-2, argv+2);
	else
	{
		fprintf(stderr, "Error: unrecognized command: %s\n", argv[1]);
		main_usage();
		return 1;
	}

	return 0;
}

int pair(int argc, char **argv)
{
	if (!argv[0])
		return pair_usage();
	else
		do_pair(argc, argv);
	return 0;
}

int do_pair(int argc, char **argv)
{
	return 0;
}

int fa2fq(int argc, char **argv)
{
	if (!argv[0])
		return fa2fq_usage();
	else
		do_fa2fq(argc, argv);
	return 0;
}

int do_fa2fq(int argc, char **argv)
{
	unsigned int i;
	char *seqFile = NULL;
	char *qualFile = NULL;
	char **seqLine;
	char **qualLine;
	FILE *seq;
	FILE *qual;
	FILE *fq;

	/* Get input file names */
	seqFile = argv[0];
	qualFile = argv[1];

	/* Open sequence file */
	if ((seq = fopen(seqFile, "r")) == NULL)
	{
		if (errno == ENOENT)
		{
			fputs("Cannot find sequence file\n", stderr);
			return 1;
		}
	}

	/* Open the quality file */
	if ((qual = fopen(qualFile, "r")) == NULL)
	{
		if (errno == ENOENT)
		{
			fputs("Cannot find quality file\n", stderr);
			return 1;
		}
	}

	/* Output stream is STDOUT */
	fq = stdout;

	/* Set up interrupt trap */
	signal(SIGINT, INThandler);

	/* Allocate memory for buffer */
	seqLine = (char**)malloc(BUFFSIZE * sizeof(char*));
	assert(seqLine);
	qualLine = (char**)malloc(BUFFSIZE * sizeof(char*));
	assert(qualLine);
	for (i=0; i < BUFFSIZE; ++i)
	{
		seqLine[i] = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
		assert(seqLine[i]);
		qualLine[i] = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));
		assert(qualLine[i]);
	}

	/* Read through both files */
	while (1)
	{
		unsigned int buffCount = 0;
		char line[MAX_LINE_LENGTH];

		/* Fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			if (fgets(line, MAX_LINE_LENGTH, seq) == NULL)
				break;
			else
				strncpy(seqLine[buffCount], &line[0], MAX_LINE_LENGTH);
			if (fgets(line, MAX_LINE_LENGTH, qual) == NULL)
				break;
			else
				strncpy(qualLine[buffCount], &line[0], MAX_LINE_LENGTH);
			++buffCount;
		}

		/* Dump buffer to stdout */
		for (i=0; i < buffCount; ++i)
		{
			if (i%2)
			{
				fputs(seqLine[i], fq);
				fputs("+\n", fq);
				fputs(qualLine[i], fq);
			}
			else
			{
				fputc('@', fq);
				fputs(seqLine[i]+1, fq);
			}
		}

		/* If we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	/* Close sequence input stream */
	fclose(seq);

	/* Close quality input stream */
	fclose(qual);

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

void INThandler(int sig)
{
	signal(sig, SIG_IGN);
	fputs("\nCtl-C caught, exiting program\n", stderr);
	exit(EXIT_SUCCESS);
}

int main_usage(void)
{
	fputc('\n', stderr);
	fputs("Usage: ngsutils [function] [options] <infile> ...\n\n", stderr);
	fputs("Functions:       fa2fq      merge fasta and quality file to fastq format\n", stderr);
	fputs("                 pair       aligned mated pairs in two fastq files\n", stderr);
	fputc('\n', stderr);
	return 0;
}

int fa2fq_usage(void)
{
	fputc('\n', stderr);
	fputs("Usage: ngsutils fa2fq [options] <fasta file> <quality file>\n\n", stderr);
	fputs("Options:        -o         name of fastq output file\n", stderr);
	fputc('\n', stderr);
	return 0;
}

int pair_usage(void)
{
	fputc('\n', stderr);
	fputs("Usage: ngsutils pair [options] <fastq_mate1> <fastq_mate2>\n\n", stderr);
	fputs("Options:        -o         prefix string for naming the output files\n", stderr);
	fputc('\n', stderr);
	return 0;
}
