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

#include "ngscmd.h"
#ifdef _MSC_VER
#include "getopt.h"
#else
#include <unistd.h>
#endif

/* constants */
#define VERSION 0.2
<<<<<<< HEAD
#define NFUNCTIONS 6
enum FUNC {FILTER, TRIM, PAIR, SCORE, RMDUP, KMER};
=======
#define NFUNCTIONS 8
enum FUNC {MAKEDB, SORT, PAIR, SCORE, FORMAT, CLEAN, RMDUP, KMER};
>>>>>>> 1d4343d0315645e10717853a2f64217068cb111e

/* function prototypes */
ngsParams *readParams(int, char**);
int mainUsage(void);
int functionUsage(int);
extern int getopt (int, char *const *, const char*);


/* globally scoped variables */
extern char *optarg;
extern int optind, opterr, optopt;

int
main(int argc, char **argv)
{
<<<<<<< HEAD
	ngsParams *p = NULL;
=======
	ngsParams *p;
>>>>>>> 1d4343d0315645e10717853a2f64217068cb111e

	if (argc < 2)
		return mainUsage();
	else
	{
		p = readParams(argc, argv);

		switch (p->func)
		{
			case FILTER:
				ngs_filter(p);
				break;
			case TRIM:
				ngs_trim(p);
				break;
			case PAIR:
				ngs_pair(p);
				break;
			case SCORE:
				ngs_score(p);
				break;
			case RMDUP:
				ngs_rmdup(p);
				break;
			case KMER:
				ngs_kmer(p);
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

	/* initialize user parameters to their default values */
	opterr = 0;
	p->flag = 0;
	p->kmer_size = 31;
	p->func = -1;

	/* assign the function */
	if (strcmp(argv[1], "filter") == 0)
		p->func = FILTER;
	else if (strcmp(argv[1], "trim") == 0)
		p->func = TRIM;
	else if (strcmp(argv[1], "pair") == 0)
		p->func = PAIR;
	else if (strcmp(argv[1], "score") == 0)
		p->func = SCORE;
	else if (strcmp(argv[1], "rmdup") == 0)
		p->func = RMDUP;
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
	while ((c = getopt(argc, argv, "ansdmrt:u:o:")) != -1)
	{
		switch(c)
		{
			case 'o':
				strcpy(p->outFilePrefix, optarg);
<<<<<<< HEAD
				if (p->func == PAIR)
=======
				if (p->func == MAKEDB)
				{
					strcpy(p->dbfile, p->outFilePrefix);
					strcat(p->dbfile, ".dbmap");
				}
				else if (p->func == FORMAT)
>>>>>>> 1d4343d0315645e10717853a2f64217068cb111e
				{
					strcpy(p->outFile1, p->outFilePrefix);
					strcpy(p->outFile2, p->outFilePrefix);
					strcat(p->outFile1, ".1.gz");
					strcat(p->outFile2, ".2.gz");
				}
				else
				{
					strcpy(p->outFile1, p->outFilePrefix);
					strcpy(p->outFile2, p->outFilePrefix);
					strcat(p->outFile1, ".1.gz");
					strcat(p->outFile2, ".2.gz");
				}
				break;
			case 'a':
				p->flag |= SCORE_ASCII;
				break;
			case 'n':
				if (p->func == SCORE)
					p->flag |= SCORE_NUM;
				else if (p->func == KMER)
					p->kmer_size = atoi(optarg);
				else
					break;
				break;
			case 's':
				p->flag |= SCORE_ILLUMINA;
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
	if ((p->func == FILTER) || (p->func == PAIR) || (p->func == RMDUP))
	{
		if (argv[optind + 1])
		{
			strcpy(p->seqFile2, argv[optind + 1]);
			p->flag |= TWO_INPUTS;
		}
		else if (p->func == RMDUP)
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


/* prints a usage message for the ngscmd program */

int
mainUsage(void)
{
	puts("\n\nUsage: ngscmd <function> [options] <fastq_mate1> <fastq_mate2>\n");
	puts("Functions:       filter     remove low quality reads");
	puts("                 trim       trim ends of reads");
	puts("                 pair       aligned mated pairs in two fastQ files");
	puts("                 score      convert Phred-scaled quality scores");
	puts("                 rmdup      remove duplicate reads");
	puts("                 kmer       count number of unique k-mers in fastQ file\n");
	return 1;
}


/* prints a custom usage message for each function */

int
functionUsage(int f)
{
	switch(f)
	{
<<<<<<< HEAD
		case FILTER:
			puts("\n\nUsage: ngscmd filter [options] <fastQ_mate1> <fastQ_mate2");
			puts("Options:        -o         prefix string for name of fastQ/fastQ/quality output files\n");
			break;
		case TRIM:
			puts("\n\nUsage: ngscmd trim [options] <fastQ file>");
			puts("Options:        -o         prefix string for name of fastA and quality file output file\n");
=======
		case MAKEDB:
			puts("\n\nUsage: ngscmd makedb [options] <fastQ_mate1> [fastq_mate2]");
			puts("Options:        -o   STR      prefix string for the name of the database map file\n");
			break;
		case SORT:
			puts("\n\nUsage: ngscmd sort [options] <database map file>");
			puts("Options:        -o   STR      prefix string for the names of the sorted fastQ output files\n");
>>>>>>> 1d4343d0315645e10717853a2f64217068cb111e
			break;
		case PAIR:
			puts("\n\nUsage: ngscmd pair [options] <database map file>");
			puts("Options:        -o   STR      prefix string for the names of the paired fastQ output files\n");
			break;
		case SCORE:
			puts("\n\nUsage: ngscmd score [options] <database map file>");
			puts("Options:        -o   STR      prefix string for the names of the fastQ output files");
			puts("                -s            convert from 33-126 scale to 64-126 scale");
			puts("                                default: 64-126 to 33-126 scale");
			puts("                -a            convert from numerical scores to ASCII");
			puts("                -n            convert from ASCII scores to numerical");
			puts("                -d            get distribution of Phred scores by position\n");
			break;
<<<<<<< HEAD
		case RMDUP:
			puts("\n\nUsage: ngscmd rmdup [options] <fastQ_mate1> <fastQ_mate2>");
			puts("Options:        -o         prefix string for name of fastQ output file\n");
=======
		case FORMAT:
			puts("\n\nUsage: ngscmd format [options] <database map file>");
			puts("Options:        -o   STR      prefix string for the names of fastQ/fastA/quality output files");
			puts("                -m            merge fastA and quality files");
			puts("                -s            split fastQ file into fastA and quality files");
			puts("                -r            reverse complement sequences\n");
			break;
		case CLEAN:
			puts("\n\nUsage: ngscmd clean [options] <database map file>");
			puts("Options:        -o   STR      prefix string for the names of the fastQ output files");
			puts("                -t   INT      trim ends of sequences to minimum length");
			puts("                -u   FLT      remove sequences with greater than this proportion of bases with N\n");
			break;
		case RMDUP:
			puts("\n\nUsage: ngscmd rmdup [options] <database map file>");
			puts("Options:        -o   STR      prefix string for the names of the fastQ output files\n");
>>>>>>> 1d4343d0315645e10717853a2f64217068cb111e
			break;
		case KMER:
			puts("\n\nUsage: ngscmd kmer [options] <database map file>");
			puts("Options:        -n   INT      k-mer size       [ default: 31 ]");
			puts("Note: kmer writes all output to STDOUT\n");
			break;
		default:
			puts("\n\nError: cannot determine specified function\n");
			exit(EXIT_FAILURE);
	}
	return 1;
}
