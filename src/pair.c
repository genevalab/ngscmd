/* pair - Align mated pairs in two fastQ input files
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
#include "uthash.h"

typedef struct _entry
{
  const char *fqid;     /* key */
  const char *seq;      /* sequence string */
  const char *qual;     /* quality string */
  UT_hash_handle hh;    /* makes this structure hashable */
} entry;

int
ngs_pair (ngs_params * p)
{
  int i = 0;
  int input_buffer_count = 0;
  char input_buffer1[BUFFSIZE][MAX_LINE_LENGTH];
  char input_buffer2[BUFFSIZE][MAX_LINE_LENGTH];
  entry *e = NULL;
  entry *tmp = NULL;
  entry *hash_fastq2 = NULL;
  gzFile input_fastq1;
  gzFile input_fastq2;
  gzFile output_fastq1;
  gzFile output_fastq2;

  /* Open the first fastQ input stream */
  if ((input_fastq1 = gzopen (p->seqfile_name1, "rb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the input fastQ file: %s.\n\n",
               p->seqfile_name1);
      abort ();
    }

  /* Open the second fastQ input stream */
  if ((input_fastq2 = gzopen (p->seqfile_name2, "rb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the second input "
               "fastQ file: %s.\n\n", p->seqfile_name2);
      abort ();
    }

  /* Open the first fastQ output stream */
  if ((output_fastq1 = gzopen (p->outfile_name1, "wb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the output fastQ file: "
               "%s.\n", p->outfile_name1);
      abort ();
    }

  /* Open the second fastQ output stream */
  if ((output_fastq2 = gzopen (p->outfile_name2, "wb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the second output fastQ file: "
               "%s.\n", p->outfile_name2);
      abort ();
    }

  /* Set up interrupt trap */
  signal (SIGINT, INThandler);

  /* Enter data from the second fastQ input file
     into a hash table */
  while (1)
    {
      /* Initialize counter for the number of lines in the buffer */
      input_buffer_count = 0;

      /* Fill up the buffer */
      while (input_buffer_count < BUFFSIZE)
        {
          /* Get line from the second fastQ input stream */
          if (gzgets (input_fastq2, input_buffer2[input_buffer_count], MAX_LINE_LENGTH)
              == Z_NULL)
            break;

          /* Iterate the counter for the number of lines
             currently in the input buffer */
          ++input_buffer_count;
        }

      for (i = 0; i < input_buffer_count; ++i)
        {
          if (i % 4 == 3)
            {
               e = (entry *) malloc (sizeof (entry));
               chomp (input_buffer2[i-3]);
               input_buffer2[i-3][strlen (input_buffer2[i-3]) - 1] = '\0';
               e->fqid = input_buffer2[i-3];
               e->seq = input_buffer2[i-2];
               e->qual = input_buffer2[i];
               HASH_ADD_KEYPTR (hh, hash_fastq2, e->fqid, strlen (e->fqid), e);
            }
        }

      /* If we are at the end of the file */
      if (input_buffer_count < BUFFSIZE)
        break;
    }

  /* Read through first fastQ input file
     and lookup IDs in hash table */
  while (1)
    {
      /* Initialize counter for the number of lines in the buffer */
      input_buffer_count = 0;

      /* Fill up the buffer */
      while (input_buffer_count < BUFFSIZE)
        {
           /* Get line from the first fastQ input stream */
           if (gzgets (input_fastq1, input_buffer1[input_buffer_count], MAX_LINE_LENGTH)
               == Z_NULL)
             break;

           /* Iterate the counter for the number of lines
              currently in the input buffer */
           ++input_buffer_count;
        }

      for (i = 0; i < input_buffer_count; ++i)
        {
          if (i % 4 == 3)
            {
              chomp (input_buffer1[i-3]);
              input_buffer1[i-3][strlen (input_buffer1[i-3]) - 1] = '\0';
              HASH_FIND_STR (hash_fastq2, input_buffer1[i-3], e);
              if (e)
                {
                   gzputs (output_fastq1, input_buffer1[i-3]);
                   gzputs (output_fastq1, "1\n");
                   gzputs (output_fastq1, input_buffer1[i-2]);
                   gzputs (output_fastq1, "+\n");
                   gzputs (output_fastq1, input_buffer1[i]);
                   gzputs (output_fastq2, e->fqid);
                   gzputs (output_fastq2, "2\n");
                   gzputs (output_fastq2, e->seq);
                   gzputs (output_fastq2, "+\n");
                   gzputs (output_fastq2, e->qual);
                 }
            }
        }

      /* If we are at the end of the file */
      if (input_buffer_count < BUFFSIZE)
        break;
    }

  /* Free the hash table contents */
  HASH_ITER (hh, hash_fastq2, e, tmp)
  {
    HASH_DEL (hash_fastq2, e);
    free (e);
  }

  /* Close the fastQ input streams */
  gzclose (input_fastq1);
  gzclose (input_fastq2);

  /* Close the fastQ output streams */
  gzclose (output_fastq1);
  gzclose (output_fastq2);

  return 0;
}
