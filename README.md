ngscmd
======

A C program to manipulate next-generation sequence data files
-------------------------------------------------------------

The `ngscmd` program depends on the zlib compression library and
the Berkeley DB API. To compile the `ngscmd` program:

	./configure

followed by

	make

The first step in using `ngscmd` is making the database files. This
can be done by running the makedb command

	./ngscmd makedb -o mydata mydata.1.fq.gz mydata.2.fq.gz

in which mydata.1.fq.gz and mydata.2.fq.gz are fastQ formatted
mate pair sequences. However, it should be noted that `ngscmd` can
work on a single fastQ input file, as well as mate pair files.
The fastQ files can be input into the `ngscmd` program in either 
compressed or uncompressed form. The `ngscmd` program will then 
make several database files for each fastQ input file and it will
make a "database map" file that contains information on the layout
of the individual databases. The "database map" file will be used
as the input for the other 7 commands of `ngscmd`.

All eight commands of the `ngscmd` program are

Command | Description
------- | -----------
makedb  | construct databases for further analysis
sort    | lexical sort of reads by identifier string
pair    | aligned mated pairs in two fastQ files
score   | manipulate or analyze Phred-scaled quality scores
format  | convert between file formats
clean   | perform a variety of cleaning procedures for reads
rmdup   | remove duplicate reads
kmer    | count number of unique k-mers in fastQ file

Although their are many programs with functionality that is
similar to `ngscmd`, the motivation for developing this program
is that most tools exist in disparate form (save perhaps the fastx-toolkit)
and many exist only as high-level scripting languages, which 
perform slowly. Thus, `ngscmd` was born out of my own frustration
trying to cobble together efficient computational tools for
processing short read data. The program is only beginning 
development, so I would be happy to have others involved or
hear from people about what other functionality would be useful
for the program (or whether they have found bugs).
