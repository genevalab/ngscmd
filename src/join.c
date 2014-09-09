/* join - Join mated pairs from two fastQ input files into single file
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



/* Complement DNA sequence */
char *compdna(char s[])
{    char *final = malloc(strlen(s) + 1);
    int i=0;
    while (s[i] != '\0')                      
    {
      if(s[i] == 'a' || s[i] == 'A')
        {
          final[i] = 'T';                       
        }                                 
      else if(s[i] == 'c' || s[i] == 'C')   
        {                                 
          final[i] = 'G';                      
        }                                 
      else if(s[i] == 'g' || s[i] == 'G')   
        {                                 
          final[i] = 'C';                      
        }                                 
      else if(s[i] == 't' || s[i] == 'T')
        {
          final[i] = 'A';
        }
      else if(s[i] == 'n' || s[i] == 'N')   
        {                                 
          final[i] = 'N';                     
        }
      ++i;
    }                                     
    final[i]=0;
    return final;
}


int
ngs_join (ngs_params * p)
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
  int j = 0;


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
  /* Open the fastQ output stream */
  if ((output_fastq1 = gzopen (p->outfile_name1, "wb")) == Z_NULL)
    {
      fprintf (stderr, "\n\nError: cannot open the output fastQ file: "
               "%s.\n", p->outfile_name1);
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
              chomp (input_buffer1[i-2]);
              chomp (input_buffer1[i]);
              
              input_buffer1[i-3][strlen (input_buffer1[i-3]) - 1] = '\0';
              HASH_FIND_STR (hash_fastq2, input_buffer1[i-3], e);
              if (e)
                {
                   chomp(e->seq);
                   chomp(e->qual);
                   strrev(e->seq);
                   strrev(e->qual);

                   gzputs (output_fastq1, input_buffer1[i-3]);
                   gzputs (output_fastq1, "\n");
                   gzputs (output_fastq1, input_buffer1[i-2]);
                     for (j = 0; j < p->gap_size; ++j)
                       {
                       gzputs (output_fastq1, "N"); /* INSERT USER DEFINED NUMBER OF Ns here */  
                       }                                     
                   gzputs (output_fastq1, compdna(e->seq)); 
                   gzputs (output_fastq1, "\n");
                   gzputs (output_fastq1, "+\n");
                   gzputs (output_fastq1, input_buffer1[i]);
                   if (p->flag & SCORE_ILLUMINA)
                     {
                     for (j = 0; j < p->gap_size; ++j)
                       {
                       gzputs (output_fastq1, "@");   /* ADD USER DEFINED NUMBER of quality scores to match with Ns:  ! for Sanger and @ for illumina */
                       } 
                     }
                   else
                   for (j = 0; j < p->gap_size; ++j)
                     {
                     gzputs (output_fastq1, "!");   /* ADD USER DEFINED NUMBER of quality scores to match with Ns:  ! for Sanger and @ for illumina */
                     } 
                   gzputs (output_fastq1, e->qual);
                   gzputs (output_fastq1, "\n");
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

  return 0;
}
