/* score - Transform Phred-scaled quality scores in a fastQ input file
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

#include "ngscmd.h"

int
ngs_score (ngs_params * p)
{
  int i = 0;
  size_t j = 0;
  size_t length = 0;
  int input_buffer_count = 0;
  char input_buffer[BUFFSIZE][MAX_LINE_LENGTH];
  gzFile input_fastq;
  gzFile output_fastq;

  /* Open the fastQ input stream */
  if ((input_fastq = gzopen (p->seqfile_name1, "rb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the input fastQ file: "
	       "%s.\n\n", p->seqfile_name1);
      abort ();
    }

  /* Open the fastQ output stream */
  if ((output_fastq = gzopen (p->outfile_name1, "wb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the output fastQ file: "
	       "%s.\n", p->outfile_name1);
      abort ();
    }

  /* Set up interrupt trap */
  signal (SIGINT, INThandler);

  /* Read through fastQ input sequence file */
  while (1)
    {
      input_buffer_count = 0;

      /* Fill up the input buffer */
      while (input_buffer_count < BUFFSIZE)
	{
	  if (gzgets (input_fastq, input_buffer[input_buffer_count],
		      MAX_LINE_LENGTH) == Z_NULL)
	    break;
	  ++input_buffer_count;
	}

      /* Dump the buffer to the output stream */
      for (i = 0; i < input_buffer_count; ++i)
	{
	  if (i % 4 == 3)
	    {
	      j = 0;
	      length = strlen (input_buffer[i]) - 1;
	      if (p->flag & SCORE_ILLUMINA)
		{
		  /* Only do Sanger to Illumina conversion */
		  while (j < length)
		    {
		      int score = input_buffer[i][j] + 31;
		      if (score > SCHAR_MAX)
			{
			  fputs ("\n\nError: the original Phred scores are "
				 "not in standard Sanger format.\n\n",
				 stderr);
			  exit (EXIT_FAILURE);
			}
		      else
			gzputc (output_fastq, score);
		      ++j;
		    }
		  gzputc (output_fastq, '\n');

		  if (p->flag & SCORE_ASCII)
		    {
		      /* Do both numerical and Sanger to Illumina
		         conversion here */
		      const char delim = ' ';
		      char *tok = NULL;
		      int score = 0;
		      tok = strtok (input_buffer[i], &delim);
		      score = atoi (tok);
		      gzputc (output_fastq, score);
		      while (tok != NULL)
			{
			  tok = strtok (NULL, &delim);
			  score = atoi (tok);
			  gzputc (output_fastq, score + 31);
			}
		      gzputc (output_fastq, '\n');
		    }
		}
	      else
		{
		  /* Only do Illumina to Sanger conversion */
		  while (j < length)
		    {
		      int score = 0;
		      score = input_buffer[i][j] - 31;
		      if ((score > SCHAR_MAX) || (score < 33))
			{
			  fputs ("\n\nError: the original Phred scores are "
				 "not in Illumina format.\n\n", stderr);
			  abort ();
			}
		      else
			gzputc (output_fastq, input_buffer[i][j] - 31);
		      ++j;
		    }
		  gzputc (output_fastq, '\n');

		  if (p->flag & SCORE_ASCII)
		    {
		      /* Do both numerical and Illumina to Sanger
		         conversion here */
		      const char delim = ' ';
		      char *tok = NULL;
		      int score = 0;
		      tok = strtok (input_buffer[i], &delim);
		      score = atoi (tok);
		      gzputc (output_fastq, score);
		      while (tok != NULL)
			{
			  tok = strtok (NULL, &delim);
			  score = atoi (tok);
			  gzputc (output_fastq, score - 31);
			}
		      gzputc (output_fastq, '\n');
		    }
		}
	    }
	  else
	    gzputs (output_fastq, input_buffer[i]);
	}

      /* If we are at the end of the file */
      if (input_buffer_count < BUFFSIZE)
	break;
    }

  /* Close the fastQ input and output streams */
  gzclose (input_fastq);
  gzclose (output_fastq);

  return 0;
}
