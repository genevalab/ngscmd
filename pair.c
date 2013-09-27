/*************************************************************************
 *
 * File: pair.c
 *
 * Description: Functions to align reads in two paired fastq files
 *
 * Author: Daniel Garrigan
 *
 *************************************************************************/
#include "ngsutils.h"

int
ngs_pair(ngsParams *p)
{
	gzFile seq1;
	gzFile seq2;
	gzFile out1;
	gzFile out2;

	/* Open sequence file 1 */
	if ((seq1 = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the first input fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open sequence file 2 */
	if ((seq2 = gzopen(p->seqFile2, "rb")) == NULL)
	{
			fputs("\n\nError: cannot open the second input fastq sequence file.\n\n", stderr);
			exit(EXIT_FAILURE);
	}

	/* Open the first fastq output stream */
	if ((out1 = gzopen(p->outFile1, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output stream for the first fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Open the second fastq output stream */
	if ((out2 = gzopen(p->outFile2, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output stream for the second fastq sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* TODO: Implement merging algorithm here */

	/* Set up interrupt trap */
	signal(SIGINT, INThandler);

	/* Close fastq 1 input stream */
	gzclose(seq1);

	/* Close fastq 2 input stream */
	gzclose(seq2);

	/* Close the output 1 stream */
	gzclose(out1);

	/* Close the output 1 stream */
	gzclose(out2);

	/* Take out the garbage */
	free(p);

	return 0;
}
