/* filter - filters out low quailty reads
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
ngs_filter (ngs_params * p)
{
  int i = 0;
  int j = 0;
  int input_buffer_count = 0;
  int count_N1 = 0;
  int count_N2 = 0;
  char *ret1 = NULL;
  char *ret2 = NULL;
  char input_buffer1[BUFFSIZE][MAX_LINE_LENGTH];
  char input_buffer2[BUFFSIZE][MAX_LINE_LENGTH];
  gzFile input_fastq1;
  gzFile input_fastq2;
  gzFile output_fastq1;
  gzFile output_fastq2;

  /* Open the first fastQ input stream */
  if ((input_fastq1 = gzopen (p->seqfile_name1, "r")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the input fastQ file: %s.\n\n",
	       p->seqfile_name1);
      abort ();
    }

  /* If specified-- open the second fastQ input stream */
  if (p->flag & TWO_INPUTS)
    {
      if ((input_fastq2 = gzopen (p->seqfile_name2, "r")) == Z_NULL)
	{
	  fprintf (stderr, "\n\nError: cannot open the second input fastQ "
		   "file: %s.\n\n", p->seqfile_name2);
	  abort ();
	}
    }

  /* Open the first fastQ output stream */
  if ((output_fastq1 = gzopen (p->outfile_name1, "w")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the output fastQ file: %s.\n",
	       p->outfile_name1);
      abort ();
    }

  /* If specified-- open the second fastQ output stream */
  if (p->flag & TWO_INPUTS)
    {
      if ((output_fastq2 = gzopen (p->outfile_name2, "w")) == Z_NULL)
	{
	  fprintf (stderr, "\n\nError: cannot open the second output "
		   "fastQ file: %s.\n", p->outfile_name2);
	  abort ();
	}
    }

  /* Set up the interrupt trap */
  signal (SIGINT, INThandler);


  /* Read through both files */
  while (1)
    {
      /* Initialize counter for the number of lines in the buffer */
      input_buffer_count = 0;

      /* Fill up the buffer */
      while (input_buffer_count < BUFFSIZE)
	{
	  /* Get line from the first fastQ input stream */
	  ret1 = gzgets (input_fastq1, input_buffer1[input_buffer_count],
			 MAX_LINE_LENGTH);

	  /* If specified-- get line from the second fastQ input stream */
	  if (p->flag & TWO_INPUTS)
	    ret2 = gzgets (input_fastq2, input_buffer2[input_buffer_count],
			   MAX_LINE_LENGTH);

	  /* Determine whether gzgets was successful */
	  if (p->flag & TWO_INPUTS)
	    {
	      /* Both gzgets calls return null--
	         we are at the end of the file */
	      if ((ret1 == Z_NULL) && (ret2 == Z_NULL))
		break;
	      /* one or the other gzgets calls returns null--
	         unequal number of records */
	      else if (((ret1 == Z_NULL) && (ret2 != Z_NULL)) ||
		       ((ret1 != Z_NULL) && (ret2 == Z_NULL)))
		{
		  fputs ("Error processing paired fastQ sequences. "
			 "Are these files properly paired?", stderr);
		  abort ();
		}
	    }
	  else if (ret1 == Z_NULL)
	    break;

	  /* Increment the counter for the number of lines
	     currently in the input buffer */
	  ++input_buffer_count;
	}

      /* Screen each sequence for number of ambiguous characters
         if record passes filter-- write to the output streams */
      for (i = 0; i < input_buffer_count; ++i)
	{
	  if (i % 4 == 1)
	    {
	      /* Reset ambiguous character counters to zero */
	      count_N1 = 0;
	      count_N2 = 0;

	      /* Count the number of ambiguous characters
	         in the first fastQ entry */
	      for (j = 0; input_buffer1[i][j]; j++)
		count_N1 += (input_buffer1[i][j] == 'N');

	      /* If specified-- count the number of ambiguous characters
	         in the second fastQ entry */
	      if (p->flag & TWO_INPUTS)
		for (j = 0; input_buffer2[i][j]; j++)
		  count_N2 += (input_buffer2[i][j] == 'N');

	      /* If two fastQ input streams are specified and both sequences
	         pass the for ambiguous characters, then write to both fastQ
	         output streams */
	      if (p->flag & TWO_INPUTS)
		{
		  if ((count_N1 <= p->num_ambig) &&
		      (count_N2 <= p->num_ambig))
		    {
		      gzputs (output_fastq1, input_buffer1[i - 1]);
		      gzputs (output_fastq1, input_buffer1[i]);
		      gzputs (output_fastq1, input_buffer1[i + 1]);
		      gzputs (output_fastq1, input_buffer1[i + 2]);
		      gzputs (output_fastq2, input_buffer2[i - 1]);
		      gzputs (output_fastq2, input_buffer2[i]);
		      gzputs (output_fastq2, input_buffer2[i + 1]);
		      gzputs (output_fastq2, input_buffer2[i + 2]);
		    }
		}
	      else
		{
		  if (count_N1 <= p->num_ambig)
		    {
		      gzputs (output_fastq1, input_buffer1[i - 1]);
		      gzputs (output_fastq1, input_buffer1[i]);
		      gzputs (output_fastq1, input_buffer1[i + 1]);
		      gzputs (output_fastq1, input_buffer1[i + 2]);
		    }
		}
	    }
	}

      /* If we are at the end of the file */
      if (input_buffer_count < BUFFSIZE)
	break;
    }

  /* Close the first fastQ input and output streams */
  gzclose (input_fastq1);
  gzclose (output_fastq1);

  /* If specified-- close the second fastQ input and output streams */
  if (p->flag & TWO_INPUTS)
    {
      gzclose (input_fastq2);
      gzclose (output_fastq2);
    }

  return 0;
}
