/* Copyright (c) 2013 Daniel Garrigan

	Permission is hereby granted, free of charge, to any person obtaining a copy of
	this software and associated documentation files (the "Software"), to deal in
	the Software without restriction, including without limitation the rights to
	use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
	the Software, and to permit persons to whom the Software is furnished to do so,
	subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
	FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
	COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
	IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

	Daniel Garrigan    dgarriga@bio.rochester.edu
*/

#include <db.h>
#include "ngslib.h"

struct dbdata
{
	char seq[MAX_LINE_LENGTH];
	char qual[MAX_LINE_LENGTH];
};

/* lexical sort of reads in a fastQ file by the identifier strings */

int
ngs_sort(ngsParams *p)
{
	int i = 0;
	int ret;
	unsigned int flags;
	char seqLine[BUFFSIZE][MAX_LINE_LENGTH];
	struct dbdata record;
	struct dbdata *buf;
	DB *dbp;
	DBC *dbcp;
	DBT key, val;
	gzFile seq;
	gzFile out;

	/* Initialize the database structure */
	ret = db_create(&dbp, NULL, 0);
	if (ret != 0)
	{
		fputs("\n\nError: cannot create database for sequences.\n\n", stderr);
		exit(EXIT_FAILURE);
	}
	flags = DB_CREATE;

	/* open sequence input file */
	if ((seq = gzopen(p->seqFile1, "rb")) == NULL)
	{
		fputs("\n\nError: cannot open the input fastQ sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* open sequence output file */
	if ((out = gzopen(p->outFile1, "wb")) == NULL)
	{
		fputs("\n\nError: cannot open the output fastQ sequence file.\n\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* open the database */
	ret = dbp->open(dbp, NULL, "tmp.db", NULL, DB_BTREE, flags, 0);
	if (ret != 0)
	{
		fputs("\n\nError: cannot open database.\n\n", stderr);
		dbp->err(dbp, ret, "tmp.db");
		dbp->close(dbp, 0);
		exit(EXIT_FAILURE);
	}

	/* set up interrupt trap */
	signal(SIGINT, INThandler);


	/* read through input sequence file */
	while (1)
	{
		/* initialize counter for the number of lines in the buffer */
		int buffCount = 0;

		/* fill up the buffer */
		while (buffCount < BUFFSIZE)
		{
			/* get line from sequence file */
			if (gzgets(seq, seqLine[buffCount], MAX_LINE_LENGTH) == Z_NULL)
				break;

			/* increment the counter for the number of lines currently in the buffer */
			++buffCount;
		}

		/* tally scores along each position in the sequence */
		for (i = 0; i < buffCount; ++i)
		{
			int j = i % 4;
			int l = 0;
			switch (j)
			{
				case 0:
					memset(&key, 0, sizeof(DBT));
					key.data = seqLine[i] + 1;
					key.size = strlen(seqLine[i] + 1);
					continue;
				case 1:
					l = strlen(seqLine[i]);
					memset(&record, 0, sizeof(struct dbdata));
					memcpy(record.seq, seqLine[i], l - 1);
					continue;
				case 2:
					continue;
				case 3:
					l = strlen(seqLine[i]);
					memset(&val, 0, sizeof(DBT));
					memcpy(record.qual, seqLine[i], l - 1);
					val.data = &record;
					val.size = sizeof(record);
					break;
				default:
					continue;
			}
			ret = dbp->put(dbp, NULL, &key, &val, 0);
			if (ret != 0)
			{
				dbp->err(dbp, ret, "DB->put");
				exit(EXIT_FAILURE);
			}
		}

		/* if we are at the end of the file */
		if (buffCount < BUFFSIZE)
			break;
	}

	if ((ret = dbp->cursor(dbp, NULL, &dbcp, 0)) != 0)
	{
		dbp->err(dbp, ret, "DB->cursor");
		exit(EXIT_FAILURE);
	}

	/* walk through the b-tree and print records */
	while ((ret = dbcp->c_get(dbcp, &key, &val, DB_NEXT)) == 0)
	{
		buf = (struct dbdata*)val.data;
		gzprintf(out, "@%s%s\n+\n%s\n", (char*)key.data, buf->seq, buf->qual);
	}
	if (ret != DB_NOTFOUND)
		dbp->err(dbp, ret, "DBcursor->get");

	/* close sequence input stream */
	gzclose(seq);

	/* close sequence output stream */
	gzclose(out);

	/* close the database */
	dbcp->c_close(dbcp);
	dbp->close(dbp, 0);

	return 0;
}
