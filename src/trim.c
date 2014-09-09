/* trim - trims the ends of reads
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
ngs_trim (ngs_params * p)
{
  int i = 0;
  int s = 0;
  int max = 0;
  int max_length = 0;
  size_t j = 0;
  size_t length = 0;
  int input_buffer_count = 0;
  char input_buffer[BUFFSIZE][MAX_LINE_LENGTH];
  gzFile input_fastq;
  gzFile output_fastq;


  /* Open the fastQ input stream */
  if ((input_fastq = gzopen (p->seqfile_name1, "rb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the input fastQ file: %s.\n\n",
	       p->seqfile_name1);
      abort ();
    }

  /* Open the fastQ output stream */
  if ((output_fastq = gzopen (p->outfile_name1, "wb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the output fastQ file: %s.\n",
	       p->outfile_name1);
      abort ();
    }

  /* Set up interrupt trap */
  signal (SIGINT, INThandler);


  /* Read through fastQ input sequence file */
  while (1)
    {
      /* Initialize counter for the number of lines in the buffer */
      input_buffer_count = 0;

      /* Fill up the buffer */
      while (input_buffer_count < BUFFSIZE)
	{
	  /* get line from the fastQ input stream */
	  if (gzgets
	      (input_fastq, input_buffer[input_buffer_count],
	       MAX_LINE_LENGTH) == Z_NULL)
	    break;

	  /* Iterate the counter for the number of lines
	     currently in the input buffer */
	  ++input_buffer_count;
	}

      /* Trim the ends of the read according to
         argmax_x{\sum_{i=x+1}^l(INT-q_i)} */
      for (i = 0; i < input_buffer_count; ++i)
	{
	  if (i % 4 == 3)
	    {
	      s = 0;
	      max = 0;
	      length = strlen (input_buffer[i]) - 1;
	      max_length = length;

	      for (j = length - 1; j >= (size_t) (p->min_read_length); --j)
		{
		  s += p->trim_quality - (input_buffer[i][j] - 33);
		  if (s < 0)
		    break;
		  if (s > max)
		    {
		      max = s;
		      max_length = j;
		    }
		}
	      input_buffer[i][max_length] = '\n';
	      input_buffer[i][max_length + 1] = '\0';
	      input_buffer[i - 2][max_length] = '\n';
	      input_buffer[i - 2][max_length + 1] = '\0';
	      gzputs (output_fastq, input_buffer[i - 3]);
	      gzputs (output_fastq, input_buffer[i - 2]);
	      gzputs (output_fastq, input_buffer[i - 1]);
	      gzputs (output_fastq, input_buffer[i]);
	    }
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
