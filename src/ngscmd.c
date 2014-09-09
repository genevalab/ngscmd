/* ngscmd - A collection of utilites for processing next-generation sequence files
   Copyright (C) 2014 Laboratory for Comparative Population Genomics

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Written by Daniel Garrigan, dgarriga@lcpg.org */

#include <getopt.h>

#include "ngscmd.h"

/* Define constants */
#define VERSION 0.2
#define NFUNCTIONS 7

enum FUNC
{
  FILTER, TRIM, PAIR, SCORE, RMDUP, KMER, JOIN
};


/* Function prototypes */
ngs_params *read_params (int, char **);
int main_usage (void);
int function_usage (int);
extern int getopt (int, char *const *, const char *);

/* Flag set by '--version' */
static int version_flag;

/* Globally scoped variables */
extern char *optarg;
extern int optind, opterr, optopt;

int
main (int argc, char **argv)
{
  ngs_params *p = NULL;

  if (argc < 2)
    return main_usage ();
  else
    {
      p = read_params (argc, argv);

      switch (p->func)
        {
           case FILTER:
             ngs_filter (p);
             break;
           case TRIM:
             ngs_trim (p);
             break;
           case PAIR:
             ngs_pair (p);
             break;
           case SCORE:
             ngs_score (p);
             break;
           case RMDUP:
             ngs_rmdup (p);
             break;
           case KMER:
             ngs_kmer (p);
             break;
           case JOIN:
             ngs_join (p);
             break;
 
          default:
             return main_usage ();
        }
    }

  free (p);
  return 0;
}

/* Read user-supplied command line parameters */

ngs_params *
read_params (int argc, char **argv)
{
  int c = 0;
  ngs_params *p = NULL;

  /* Allocate memory for parameter data structure */
  p = (ngs_params *) malloc (sizeof (ngs_params));
  if (p == NULL)
    {
      puts ("Error: memory allocation failure for user parameter "
            "data structure.");
      exit (EXIT_FAILURE);
    }

  /* Initialize user parameters to their default values */
  opterr = 0;
  version_flag = 0;
  p->flag = 0;
  p->kmer_size = 31;
  p->func = -1;
  p->num_ambig = 0;
  p->min_read_length = 25;
  p->trim_quality = 13;
  p->gap_size = 0;      /*AJG ADDED */

  /* Assign the function */
  if (strcmp (argv[1], "filter") == 0)
    p->func = FILTER;
  else if (strcmp (argv[1], "trim") == 0)
    p->func = TRIM;
  else if (strcmp (argv[1], "pair") == 0)
    p->func = PAIR;
  else if (strcmp (argv[1], "score") == 0)
    p->func = SCORE;
  else if (strcmp (argv[1], "rmdup") == 0)
    p->func = RMDUP;
  else if (strcmp (argv[1], "kmer") == 0)
    p->func = KMER;
  else if (strcmp (argv[1], "join") == 0)
    p->func = JOIN;
  else
    {
      printf ("Error: the function \"%s\" is not recognized\n", argv[1]);
      main_usage ();
      abort ();
    }

  /* Increment the argument variables */
  --argc;
  ++argv;

  if (argv == NULL)
    function_usage (p->func);

  /* Read the command line options */
  while (1)
    {
      static struct option long_options[] =
        {
          {"version", no_argument, &version_flag, 1},
          {"ascii", no_argument, 0, 'a'},
          {"illumina", no_argument, 0, 'i'},
          {"number", no_argument, 0, 'n'},
          {"prefix", required_argument, 0, 'p'},
          {"allowed-N", required_argument, 0, 'w'},
          {"kmer-size", required_argument, 0, 'k'},
          {"trim-quality", required_argument, 0, 'q'},
          {"min-length", required_argument, 0, 'm'},
          {"gap-length", required_argument, 0, 'g'},
          {0, 0, 0, 0}
        };
      int option_index = 0;

      /* Print version */
      if (version_flag)
        {
          puts ("ngscmd VERSION");
          abort ();
        }

      c = getopt_long (argc, argv, "ainp:w:k:q:m:g:",
                       long_options, &option_index);

      if (c == -1)
        break;

      switch (c)
        {
        case 'a':
          p->flag |= SCORE_ASCII;
          break;

        case 'i':
          p->flag |= SCORE_ILLUMINA;
          break;

        case 'n':
          p->flag |= SCORE_NUM;
          break;

        case 'p':
          strcpy (p->outfile_prefix, optarg);
          strcpy (p->outfile_name1, p->outfile_prefix);
          strcpy (p->outfile_name2, p->outfile_prefix);
          strcat (p->outfile_name1, ".1.gz");
          strcat (p->outfile_name2, ".2.gz");
          break;

        case 'k':
          p->kmer_size = atoi (optarg);
          break;

        case 'w':
          p->num_ambig = atoi (optarg);
          break;

        case 'm':
          p->min_read_length = atoi (optarg);
          if (p->min_read_length < 10)
            {
              fputs ("Error: -m needs to be greater than 9", stderr);
              abort ();
            }
          break;

        case 'q':
          p->trim_quality = atoi (optarg);
          if (p->trim_quality < 1)
            {
              fputs ("Error: -q needs to be greater than 1", stderr);
              abort ();
            }
          break;

        case 'g':
          p->gap_size = atoi (optarg);
          break;

        case '?':
          if (optopt == 'o')
            printf ("Error: the option -%c requires an argument.\n", optopt);
          else if (isprint (optopt))
            printf ("Error: unknown option \"-%c\".\n", optopt);
          else
            printf ("Error: unknown option character '\\x%x'.\n", optopt);
          abort ();

         default:
          function_usage (p->func);
          abort ();
        }
    }

  /* Get the remaining non-optioned command line arguments */
  if (optind < argc)
    {
      /* Get the first non-optioned argument */
      strcpy (p->seqfile_name1, argv[optind++]);
      if (optind < argc)
        {
          strcpy (p->seqfile_name2, argv[optind]);
          p->flag |= TWO_INPUTS;
        }
    }
  else
    {
      puts ("Error: need the input fastQ sequence file name as mandatory "
            "argument.\n");
      function_usage (p->func);
      abort ();
    }

  /* For the FILTER, PAIR, RMDUP and JOIN functions
     make sure there was second non-optioned argument */
  if ((p->func == FILTER) || (p->func == PAIR) || (p->func == RMDUP) || (p->func == JOIN))
    {
      if (p->seqfile_name2 == NULL)
        {
           puts
            ("Error: need the name of the second input fastQ sequence as a "
             "mandatory argument.\n");
          function_usage (p->func);
          abort ();
        }
    }

  return p;
}

/* Handler for an interrupt signal */

void
INThandler (int sig)
{
  signal (sig, SIG_IGN);
  fputs ("\nCtl-C caught... exiting program\n", stderr);
  exit (EXIT_SUCCESS);
}

/* Print a usage message for the ngscmd program */

int
main_usage (void)
{
  puts ("Usage: ngscmd [COMMAND] [OPTION]... [FILE]...");
  puts ("Utilities for cleaning next-generation sequence files");
  putchar ('\n');
  puts ("Available commands:");
  puts ("  filter   remove reads with ambiguous base calls");
  puts ("  trim     trim ends of reads based on quality");
  puts ("  pair     aligned mated pairs in two fastQ files");
  puts ("  score    convert Phred-scaled quality scores");
  puts ("  rmdup    remove duplicate reads");
  puts ("  kmer     count number of unique k-mers in fastQ file");
  puts ("  join     aligned mated pairs joined");
  putchar ('\n');
  return 1;
}

/* Print a custom usage message for each function */

int
function_usage (int f)
{
  switch (f)
    {
    case FILTER:
      puts ("Usage: ngscmd filter [OPTION]... [FILE]...");
      puts ("Remove reads that contain more than a specific number of ambiguity characters");
      putchar ('\n');
      puts
     ("Mandatory arguments to long options are mandatory for short options too.");
      puts
     ("  -p, --prefix=Name       prefix string for name of fastQ output file(s)");
      puts
     ("  -w, --allowed-N         number of IUPAC ambiguity characters (N)");
     ("                          tolerated in a read [default: 0]");
      break;
    case TRIM:
      puts ("Usage: ngscmd trim [OPTION]... [FILE]...");
      puts ("Trim low quality bases from the end of reads");
      putchar ('\n');
      puts
     ("Mandatory arguments to long options are mandatory for short options too.");
      puts
     ("  -p, --prefix=Name          prefix string for name of fastQ output file(s)");
      puts
     ("  -m, --min-length=Length    minimum read length to retain after trimming [default: 25]");
      puts
     ("  -q, --trim-quality=Score   quality parameter [default: 13]");
      break;
    case PAIR:
      puts ("Usage: ngscmd pair [OPTION]... [FILE]...");
      puts ("Restore propering ordering in two fastQ files to preserve mate pairs");
      putchar ('\n');
      puts
     ("Mandatory arguments to long options are mandatory for short options too.");
      puts ("  -p, --prefix=Name       prefix string for name of fastQ output files");
      break;
    case SCORE:
      puts ("Usage: ngscmd score [OPTION]... [FILE]");
      puts ("Convert fastQ quality scores");
      putchar ('\n');
      puts
     ("Mandatory arguments to long options are mandatory for short options too.");
      puts ("  -p, --prefix=Name       prefix string for name of fastQ output file");
      puts ("  -i, --illumina          convert from 33-126 scale to 64-126 scale");
      puts ("                          [default: 64-126 to 33-126 scale]");
      puts ("  -a, --ascii             convert from numerical scores to ASCII");
      puts ("  -n, --number            convert from ASCII scores to numerical");
      break;
    case RMDUP:
      puts ("Usage: ngscmd rmdup [OPTION]... [FILE]...");
      puts ("Remove duplicate reads");
      putchar ('\n');
      puts
     ("Mandatory arguments to long options are mandatory for short options too.");
      puts
     ("Options: -p, --prefix=Name   prefix string for name of fastQ output file(s)");
      break;
    case KMER:
      puts ("Usage: ngscmd kmer [OPTION]... [FILE]...");
      puts ("Counts all k-mers from reads in fastQ files");
      putchar ('\n');
      puts
     ("Mandatory arguments to long options are mandatory for short options too.");
      puts ("  -k, --kmer-size=Size    Length in base pairs of k-mers to count [default: 31]");
      break;
    case JOIN:
      puts ("Usage: ngscmd join [OPTION]... [FILE]...");
      puts ("Sorts mated pairs and prints them each as a single combined read");
      putchar ('\n');
      puts
     ("Mandatory arguments to long options are mandatory for short options too.");
      puts ("  -p, --prefix=Name       prefix string for name of fastQ output files");
      puts ("  -g, --gap-size          the number of ambigous characters to insert between mate pairs");
      break;
    default:
      puts ("Error: unrecognized function");
      putchar ('\n');
      abort ();
    }
  putchar ('\n');
  puts ("Report ngscmd bugs to dgarriga@lcpg.org");
  return 1;
}
