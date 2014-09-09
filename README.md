ngscmd
======

A C program to manipulate next-generation sequence data files
-------------------------------------------------------------

The `ngscmd` program depends on the zlib compression library.
To compile the `ngscmd` program:

	./configure

followed by

	make

The `ngscmd` program can work on a single fastQ input file, as well 
as mate pair files. The fastQ files can be input into the `ngscmd` 
program in either compressed or uncompressed form. 

All seven commands of the `ngscmd` program are

Command | Description
------- | -----------
filter  |   remove low quality reads
trim    |   trim ends of reads
pair    |   align mated pairs in two fastQ files
score   |   convert Phred-scaled quality scores
rmdup   |   remove duplicate reads
kmer    |   count number of unique k-mers in fastQ file
join    |   join aligned mated pairs into single file

Although their are many programs with functionality that is
similar to `ngscmd`, the motivation for developing this program
is that most tools exist in disparate form (save perhaps the `fastx-toolkit`)
and many exist only as high-level scripting languages, which 
perform slowly. Thus, `ngscmd` was born out of my own frustration
trying to cobble together efficient computational tools for
processing short read data. The program is only beginning 
development, so I would be happy to have others involved or
hear from people about what other functionality would be useful
for the program (or whether they have found bugs).
