#include "ngsutils.h"

#define CONVERT_STD 0x1
#define CONVERT_NUM 0x2

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
	int convert_flag = 0;
	char seqFile[FILE_NAME_LENGTH];
	char outFile[FILE_NAME_LENGTH];
	gzFile fastq_in;
	gzFile fastq_out;

   /* Read command line options */
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "sio:")) != -1)
	{
		switch(c)
		{
			case 'o':
				strcpy(outFile, optarg);
				strcat(outFile, ".gz");
				break;
			case 'i':
				convert_flag |= CONVERT_NUM;
				break;
			case 's':
				convert_flag |= CONVERT_STD;
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
		fputs("\nNeed input fastq sequence file name\n", stderr);
		convert_usage();
		exit(EXIT_FAILURE);
	}

	/* Open sequence file */
	if ((fastq_in = gzopen(seqFile, "rb")) == NULL)
	{
		fputs("Error opening the fastq sequence file\n", stderr);
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

	/* Close fastq input sequence file stream */
	gzclose(fastq_in);

	/* Close fastq output sequence file stream */
	gzclose(fastq_out);

	return 0;
}

int convert_usage(void)
{
	fputc(0x0a, stderr);
	fputs("Usage: ngsutils convert [options] <fastq file>\n\n", stderr);
	fputs("Options:        -o         prefix string for name of fastq output file\n", stderr);
	fputs("                -s         convert from 64-126 scale to 33-126 scale\n", stderr);
	fputs("                -i         convert from numerical to ascii\n", stderr);
	fputc(0x0a, stderr);
	return 0;
}
