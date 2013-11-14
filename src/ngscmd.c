/* Copyright (c) 2013 Daniel Garrigan
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Daniel Garrigan    dgarriga@bio.rochester.edu
 */

#include "ngscmd.h"
#ifdef _MSC_VER
#include "getopt.h"
#else
#include <unistd.h>
#endif

/* constants */
#define VERSION 0.2
#define NFUNCTIONS 6

enum FUNC
{
    FILTER, TRIM, PAIR, SCORE, RMDUP, KMER
};


/* function prototypes */
ngsParams *read_params(int, char**);
int main_usage(void);
int function_usage(int);
extern int getopt(int, char *const *, const char*);


/* globally scoped variables */
extern char *optarg;
extern int optind, opterr, optopt;

int
main(int argc, char **argv)
{
    ngsParams *p = NULL;

    if (argc < 2)
        return main_usage();
    else
    {
        p = read_params(argc, argv);

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
            return main_usage();
        }
    }

    /* take out the garbage */
    free(p);

    return 0;
}

/* read user-supplied command line parameters */

ngsParams*
read_params(int argc, char **argv)
{
    int c = 0;
    ngsParams *p = NULL;

    /* allocate memory for parameter data structure */
    p = (ngsParams*) malloc(sizeof (ngsParams));
    if (p == NULL)
    {
        puts("Error: memory allocation failure for user parameter "
                "data structure.");
        exit(EXIT_FAILURE);
    }

    /* initialize user parameters to their default values */
    opterr = 0;
    p->flag = 0;
    p->kmer_size = 31;
    p->func = -1;
    p->num_ambig = 0;
    p->min_read_length = 25;
    p->trim_quality = 13;

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
        printf("Error: the function \"%s\" is not recognized\n", argv[1]);
        main_usage();
        exit(EXIT_FAILURE);
    }

    /* increment the argument variables */
    --argc;
    ++argv;

    if (argv == NULL)
        function_usage(p->func);

    /* read the command line options */
    while ((c = getopt(argc, argv, "asro:n:m:q:")) != -1)
    {
        switch (c)
        {
        case 'a':
            p->flag |= SCORE_ASCII;
            break;
        case 's':
            p->flag |= SCORE_ILLUMINA;
            break;
        case 'r':
            p->flag |= SCORE_NUM;
            break;
        case 'o':
            strcpy(p->outFilePrefix, optarg);
            strcpy(p->outFile1, p->outFilePrefix);
            strcpy(p->outFile2, p->outFilePrefix);
            strcat(p->outFile1, ".1.gz");
            strcat(p->outFile2, ".2.gz");
            break;
        case 'n':
            if (p->func == KMER)
                p->kmer_size = atoi(optarg);
            else if (p->func == FILTER)
                p->num_ambig = atoi(optarg);
            else
                break;
            break;
        case 'm':
            p->min_read_length = atoi(optarg);
            if (p->min_read_length < 10)
            {
                fputs("Error: -m needs to be greater than 9", stderr);
                exit(EXIT_FAILURE);
            }
            break;
        case 'q':
            p->trim_quality = atoi(optarg);
            if (p->trim_quality < 1)
            {
                fputs("Error: -q needs to be greater than 1", stderr);
                exit(EXIT_FAILURE);
            }
            break;
        case '?':
            if (optopt == 'o')
                printf("Error: the option -%c requires an argument.\n", optopt);
            else if (isprint(optopt))
                printf("Error: unknown option \"-%c\".\n", optopt);
            else
                printf("Error: unknown option character '\\x%x'.\n",
                       optopt);
            exit(EXIT_FAILURE);
        default:
            function_usage(p->func);
            exit(EXIT_FAILURE);
        }
    }

    /* get first non-optioned argument */
    if (argv[optind])
        strcpy(p->seqFile1, argv[optind]);
    else
    {
        puts("Error: need the input fastQ sequence file name as mandatory "
                "argument.\n");
        function_usage(p->func);
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
        else if ((p->func == RMDUP) || (p->func == PAIR))
        {
            puts("Error: need the name of the second input fastQ sequence as a "
                    "mandatory argument.\n");
            function_usage(p->func);
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
main_usage(void)
{
    puts("Usage: ngscmd <function> [options] <fastq_mate1>... <fastq_mate2>");
    putchar('\n');
    puts("Functions: filter   remove reads with ambiguous base calls");
    puts("           trim     trim ends of reads based on quality");
    puts("           pair     aligned mated pairs in two fastQ files");
    puts("           score    convert Phred-scaled quality scores");
    puts("           rmdup    remove duplicate reads");
    puts("           kmer     count number of unique k-mers in fastQ file");
    putchar('\n');
    return 1;
}

/* prints a custom usage message for each function */

int
function_usage(int f)
{
    switch (f)
    {
    case FILTER:
        puts("Usage: ngscmd filter [options] <fastQ_mate1> ... <fastQ_mate2>");
        putchar('\n');
        puts("Options: -o STR  prefix string for name of fastQ output file(s)");
        puts("         -n INT  number of N's tolerated in a read [default: 0]");
        putchar('\n');
        break;
    case TRIM:
        puts("Usage: ngscmd trim [options] <fastQ_mate1>");
        putchar('\n');
        puts("Options: -o STR  prefix string for name of fastQ output file");
        puts("         -m INT  minimum read length [default: 25]");
        puts("         -q INT  quality parameter   [default: 13]");
        putchar('\n');
        break;
    case PAIR:
        puts("Usage: ngscmd pair [options] <fastQ_mate1> <fastQ_mate2>");
        putchar('\n');
        puts("Options: -o STR  prefix string for name of fastQ output files");
        putchar('\n');
        break;
    case SCORE:
        puts("Usage: ngscmd score [options] <fastQ input file>");
        putchar('\n');
        puts("Options: -o STR  prefix string for name of fastQ output file");
        puts("         -s      convert from 33-126 scale to 64-126 scale");
        puts("                   [default: 64-126 to 33-126 scale]");
        puts("         -a      convert from numerical scores to ASCII");
        puts("         -r      convert from ASCII scores to numerical");
        putchar('\n');
        break;
    case RMDUP:
        puts("Usage: ngscmd rmdup [options] <fastQ_mate1> ... <fastQ_mate2>");
        putchar('\n');
        puts("Options: -o STR  prefix string for name of fastQ output file(s)");
        putchar('\n');
        break;
    case KMER:
        puts("Usage: ngscmd kmer [options] <fastQ input file>");
        putchar('\n');
        puts("Options: -n INT  k-mer size [default: 31]");
        putchar('\n');
        puts("Note: kmer writes all output to STDOUT");
        putchar('\n');
        break;
    default:
        puts("Error: unrecognized function");
        putchar('\n');
        exit(EXIT_FAILURE);
    }
    return 1;
}
