/* rmdup - Remove duplicate sequences in fastQ files
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
ngs_rmdup (ngs_params * p)
{
  int i = 0;
  int j = 0;
  int input_buffer_count = 0;
  size_t k = 0;
  size_t len = 0;
  char input_buffer1[BUFFSIZE][MAX_LINE_LENGTH];
  char input_buffer2[BUFFSIZE][MAX_LINE_LENGTH];
  gzFile input_fastq1;
  gzFile input_fastq2;
  gzFile output_fastq1;
  gzFile output_fastq2;


  /* open the first fastQ input stream */
  if ((input_fastq1 = gzopen (p->seqfile_name1, "rb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the input fastQ file: "
	       "%s.\n\n", p->seqfile_name1);
      abort ();
    }

  /* if specified-- open the second fastQ input stream */
  if (p->flag & TWO_INPUTS)
    {
      if ((input_fastq2 = gzopen (p->seqfile_name2, "r")) == Z_NULL)
	{
	  fprintf (stderr, "\n\nError: cannot open the second input "
		   "fastQ file: %s.\n\n", p->seqfile_name2);
	  abort ();
	}
    }

  /* open the first fastQ output stream */
  if ((output_fastq1 = gzopen (p->outfile_name1, "wb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the output fastQ file: "
	       "%s.\n", p->outfile_name1);
      abort ();
    }

  /* if specified-- open the second fastQ output stream */
  if (p->flag & TWO_INPUTS)
    {
      if ((output_fastq2 = gzopen (p->outfile_name2, "w")) == Z_NULL)
	{
	  fprintf (stderr, "\n\nError: cannot open the second output "
		   "fastQ file: %s.\n", p->outfile_name2);
	  abort ();
	}
    }

  /* set up interrupt trap */
  signal (SIGINT, INThandler);

  /* read through input fastQ files */
  while (1)
    {
      /* initialize counter for the number of lines in the buffer */
      input_buffer_count = 0;

      /* fill up the buffer */
      while (input_buffer_count < BUFFSIZE)
	{
	  /* get line from first fastQ input stream */
	  if (gzgets (input_fastq1, input_buffer1[input_buffer_count],
		      MAX_LINE_LENGTH) == Z_NULL)
	    break;

	  /* get line from second fastQ input stream */
	  if (p->flag & TWO_INPUTS)
	    {
	      if (gzgets (input_fastq2, input_buffer2[input_buffer_count],
			  MAX_LINE_LENGTH) == Z_NULL)
		break;
	    }

	  /* increment the counter for the number of lines
	     currently in the input buffer */
	  ++input_buffer_count;
	}

      /* reverse complement bases and reverse quality scores */
      for (i = 0; i < input_buffer_count; ++i)
	{
	  /* TODO: Remove duplicates here-- use B-tree? */
	}

      /* if we are at the end of the file */
      if (input_buffer_count < BUFFSIZE)
	break;
    }

  /* close the first fastQ input and output streams */
  gzclose (input_fastq1);
  gzclose (output_fastq1);

  /* if specified-- close the second fastQ input and output streams */
  if (p->flag & TWO_INPUTS)
    {
      gzclose (input_fastq2);
      gzclose (output_fastq2);
    }

  return 0;
}
