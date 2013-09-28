/* Copyright (c) 2013 Daniel Garrigan

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
	the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
	FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
	IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	Daniel Garrigan    dgarriga@bio.rochester.edu
*/

#include "ngslib.h"
#ifdef _MSC_VER
#include "getopt.h"
#include <io.h>
#define open _open
#define close _close
#else
#include <unistd.h>
#endif

/* constants */
#define VERSION 0.1
#define NFUNCTIONS 9

/* function prototypes */
ngsParams *readParams(int, char**);
int mainUsage(void);
int functionUsage(int);
extern int getopt (int, char *const *, const char*);


/* globally scoped variables */
extern char *optarg;
extern int optind, opterr, optopt;


/* entry point for the NGSutils program */

int
main(int argc, char **argv)
{
	int ifd;
	int iifd;
	int ofd;
	int oofd;
	ngsParams *p;

	if (argc < 2)
		return mainUsage();
	else
	{
		p = readParams(argc, argv);

		switch (p->func)
		{
			case FA2FQ:
				ifd = open(p->seqFile1, O_RDONLY);
				iifd = open(p->seqFile2, O_RDONLY);
#ifdef _MSC_VER
				ofd = open(p->outFile1, _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
#else
				ofd = open(p->outFile1, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
				if ((ifd == -1) || (iifd == -1))
					perror("Opening input files failed.");
				if (ofd == -1)
					perror("Opening output file failed.");
				ngs_fa2fq(ifd, iifd, ofd);
				close(ifd);
				close(iifd);
				close(ofd);
				break;
			case PAIR:
				ifd = open(p->seqFile1, O_RDONLY);
				iifd = open(p->seqFile2, O_RDONLY);
#ifdef _MSC_VER
				ofd = open(p->outFile1, _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
				oofd = open(p->outFile2, _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
#else
				ofd = open(p->outFile1, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
				oofd = open(p->outFile2, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
				if ((ifd == -1) || (iifd == -1))
					perror("Opening input files failed.");
				if ((ofd == -1) || (oofd == -1))
					perror("Opening output files failed.");
				ngs_pair(ifd, iifd, ofd, oofd);
				close(ifd);
				close(iifd);
				close(ofd);
				close(oofd);
				break;
			case CONVERT:
				ifd = open(p->seqFile1, O_RDONLY);
#ifdef _MSC_VER
				ofd = open(p->outFile1, _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
#else
				ofd = open(p->outFile1, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
				if (ifd == -1)
					perror("Opening input file failed.");
				if (ofd == -1)
					perror("Opening output file failed.");
				ngs_convert(ifd, ofd);
				close(ifd);
				close(ofd);
				break;
			case CLEAN:
				ifd = open(p->seqFile1, O_RDONLY);
#ifdef _MSC_VER
				ofd = open(p->outFile1, _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
#else
				ofd = open(p->outFile1, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
				if (ifd == -1)
					perror("Opening input file failed.");
				if (ofd == -1)
					perror("Opening output file failed.");
				ngs_clean(ifd, ofd);
				close(ifd);
				close(ofd);
				break;
			case BYPOS:
				ifd = open(p->seqFile1, O_RDONLY);
				if (ifd == -1)
					perror("Opening input file failed.");
				ngs_bypos(ifd);
				close(ifd);
				break;
			case SORT:
				ifd = open(p->seqFile1, O_RDONLY);
#ifdef _MSC_VER
				ofd = open(p->outFile1, _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
#else
				ofd = open(p->outFile1, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
				if (ifd == -1)
					perror("Opening input file failed.");
				if (ofd == -1)
					perror("Opening output file failed.");
				ngs_sort(ifd, ofd);
				close(ifd);
				close(ofd);
				break;
			case REVCOM:
				ifd = open(p->seqFile1, O_RDONLY);
#ifdef _MSC_VER
				ofd = open(p->outFile1, _O_WRONLY | _O_CREAT | _O_TRUNC, _S_IREAD | _S_IWRITE);
#else
				ofd = open(p->outFile1, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
#endif
				if (ifd == -1)
					perror("Opening input file failed.");
				if (ofd == -1)
					perror("Opening output file failed.");
				ngs_revcom(ifd, ofd);
				close(ifd);
				close(ofd);
				break;
			case KMER:
				ifd = open(p->seqFile1, O_RDONLY);
				if (ifd == -1)
					perror("Opening input file failed.");
				ngs_kmer(ifd);
				close(ifd);
				break;
			default:
				return mainUsage();
		}
	}

	/* take out the garbage */
	free(p);

	return 0;
}


/* read user-supplied command line parameters */

ngsParams*
readParams(int argc, char **argv)
{
	int c = 0;
	ngsParams *p = NULL;

	/* allocate memory for parameter data structure */
	p = (ngsParams*) malloc(sizeof(ngsParams));
	if (p == NULL)
	{
		puts("\n\nError: memory allocation failure for user parameter data structure.\n\n");
		exit (EXIT_FAILURE);
	}

	/* initialize some variables to default values */
	opterr = 0;
	p->flag = 0;
	p->kmer_size = 31;
	p->func = -1;

	/* assign the function */
	if (strcmp(argv[1], "fa2fq") == 0)
		p->func = FA2FQ;
	else if (strcmp(argv[1], "pair") == 0)
		p->func = PAIR;
	else if (strcmp(argv[1], "convert") == 0)
		p->func = CONVERT;
	else if (strcmp(argv[1], "clean") == 0)
		p->func = CLEAN;
	else if (strcmp(argv[1], "bypos") == 0)
		p->func = BYPOS;
	else if (strcmp(argv[1], "sort") == 0)
		p->func = SORT;
	else if (strcmp(argv[1], "revcom") == 0)
		p->func = REVCOM;
	else if (strcmp(argv[1], "kmer") == 0)
		p->func = KMER;
	else
	{
		printf("\n\nError: the function \"%s\" is not recognized\n", argv[1]);
		mainUsage();
		exit(EXIT_FAILURE);
	}

	/* increment the argument variables */
	--argc;
	++argv;

	if (argv == NULL)
		functionUsage(p->func);

   /* read the command line options */
	while ((c = getopt(argc, argv, "anso:")) != -1)
	{
		switch(c)
		{
			case 'o':
				strcpy(p->outFilePrefix, optarg);
				if (p->func == FQ2FA)
				{
					strcpy(p->outFile1, p->outFilePrefix);
					strcpy(p->qualFile, p->outFilePrefix);
					strcat(p->outFile1, ".fq.gz");
					strcat(p->qualFile, ".qual.gz");
				}
				else if (p->func == PAIR)
				{
					strcpy(p->outFile1, p->outFilePrefix);
					strcpy(p->outFile2, p->outFilePrefix);
					strcat(p->outFile1, ".1.gz");
					strcat(p->outFile2, ".2.gz");
				}
				else
				{
					strcpy(p->outFile1, p->outFilePrefix);
					strcat(p->outFile1, ".fq.gz");
				}
				break;
			case 'a':
				p->flag |= CONVERT_NUM;
				break;
			case 'n':
				if (p->func == CONVERT)
					p->flag |= CONVERT_ASCII;
				else if (p->func == KMER)
					p->kmer_size = atoi(optarg);
				else
					break;
				break;
			case 's':
				p->flag |= CONVERT_REV;
				break;
			case '?':
				if (optopt == 'o')
					printf("\n\nError: the option -%c requires an argument.\n\n", optopt);
				else if (isprint(optopt))
					printf("\n\nError: unknown option \"-%c\".\n\n", optopt);
				else
					printf("\n\nError: unknown option character '\\x%x'.\n\n", optopt);
				exit(EXIT_FAILURE);
			default:
				functionUsage(p->func);
				exit(EXIT_FAILURE);
		}
	}

	/* get first non-optioned argument */
	if (argv[optind])
		strcpy(p->seqFile1, argv[optind]);
	else
	{
		puts("\n\nError: need the input fastQ sequence file name as mandatory argument.\n\n");
		functionUsage(p->func);
		exit(EXIT_FAILURE);
	}

	/* get the second non-optioned argument */
	if ((p->func == FA2FQ) || (p->func == PAIR))
	{
		if (argv[optind + 1])
			strcpy(p->seqFile2, argv[optind + 1]);
		else
		{
			puts("\n\nError: need the name of the second input fastQ sequence as a mandatory argument.\n\n");
			functionUsage(p->func);
			exit(EXIT_FAILURE);
		}
	}
	
	return p;
}

/* handler for an interrupt signal */

void
INThandler(int sig)
{
	signal(sig, SIG_IGN);
	fputs("\nCtl-C caught... exiting program\n", stderr);
	exit(EXIT_SUCCESS);
}


/* prints a usage message for the NGSutils program */

int
mainUsage(void)
{
	puts("\n\nUsage: NGSutils <function> [options] <infile> ...\n");
	puts("Functions:       fa2fq      convert from fastA and quality files to fastQ format");
	puts("                 fq2fa      convert from fastQ format to fastA and quality files");
	puts("                 pair       aligned mated pairs in two fastQ files");
	puts("                 convert    convert Phred-scaled quality scores");
	puts("                 clean      perform a variety of cleaning procedures for reads");
	puts("                 bypos      show average quality by sequence position");
	puts("                 sort       lexical sort of reads by identifier string");
	puts("                 revcom     reverse complement bases in fastQ file");
	puts("                 kmer       count number of unique k-mers in fastQ file\n");
	return 1;
}


/* prints a custom usage message for each function */
int
functionUsage(int f)
{
	switch(f)
	{
		case FA2FQ:
			puts("\n\nUsage: NGSutils fa2fq [options] <fastA/Q input file> <quality file>");
			puts("Options:        -o         prefix string for name of fastQ/fastQ/quality output files\n");
			break;
		case FQ2FA:
			puts("\n\nUsage: NGSutils fq2fa [options] <fastQ file>");
			puts("Options:        -o         prefix string for name of fastA and quality file output file\n");
			break;
		case PAIR:
			puts("\n\nUsage: NGSutils pair [options] <fastQ_mate1> <fastQ_mate2>");
			puts("Options:        -o         prefix string for naming both output files\n");
			break;
		case CONVERT:
			puts("\n\nUsage: NGSutils convert [options] <fastQ file>");
			puts("Options:        -o         prefix string for name of fastQ output file");
			puts("                -s         convert from 33-126 scale to 64-126 scale");
			puts("                           default: 64-126 to 33-126 scale");
			puts("                -a         convert from numerical scores to ASCII");
			puts("                -n         convert from ASCII scores to numerical\n");
			break;
		case CLEAN:
			puts("\n\nUsage: NGSutils clean [options] <fastQ file>");
			puts("Options:        -o         prefix string for name of fastQ output file\n");
			break;
		case BYPOS:
			puts("\n\nUsage: NGSutils bypos <fastQ file>");
			puts("Note: bypos writes all output to STDOUT\n");
			break;
		case SORT:
			puts("\n\nUsage: NGSutils sort [options] <fastQ file>");
			puts("Options:        -o         prefix string for name of fastQ output file\n");
			break;
		case REVCOM:
			puts("\n\nUsage: NGSutils revcom [options] <fastQ file>");
			puts("Options:        -o         prefix string for name of fastQ output file\n");
			break;
		case KMER:
			puts("\n\nUsage: NGSutils kmer [options] <fastQ file>\n");
			puts("Options:        -n         k-mer size       [ default: 31 ]");
			puts("Note: kmer writes all output to STDOUT\n");
			break;
		default:
			puts("\n\nError: cannot determine specified function\n");
			exit(EXIT_FAILURE);
	}
	return 1;
}
