/* kmer - Counts the number of unique k-mers in a single fastQ file
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
ngs_kmer (ngs_params * p)
{
  int i = 0;
  int input_buffer_count = 0;
  char io_buffer[BUFFSIZE][MAX_LINE_LENGTH];
  gzFile input_fastq;


  /* Open the first fastQ input stream */
  if ((input_fastq = gzopen (p->seqfile_name1, "rb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the input fastQ file: "
	       "%s.\n\n", p->seqfile_name1);
      abort ();
    }

  /* Set up interrupt trap */
  signal (SIGINT, INThandler);

  /* Read through input sequence file */
  while (1)
    {
      /* Initialize counter for the number of lines in the buffer */
      input_buffer_count = 0;

      /* Fill up the buffer */
      while (input_buffer_count < BUFFSIZE)
	{
	  /* Get line from the fastQ input stream */
	  if (gzgets
	      (input_fastq, io_buffer[input_buffer_count],
	       MAX_LINE_LENGTH) == Z_NULL)
	    break;

	  /* Iterate the counter for the number of lines currently
	     in the buffer */
	  ++input_buffer_count;
	}

      /* Tally scores along each position in the sequence */
      for (i = 0; i < input_buffer_count; ++i)
	{
	  /* TODO: implement kmer counting algorithm */
	}

      /* If we are at the end of the file */
      if (input_buffer_count < BUFFSIZE)
	break;
    }

  /* Close the fastQ input stream */
  gzclose (input_fastq);

  return 0;
}
