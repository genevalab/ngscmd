bin_PROGRAMS = ngscmd
ngscmd_SOURCES = ngscmd.c filter.c trim.c pair.c score.c rmdup.c kmer.c join.c
ngscmd_CFLAGS = -Wall -D MAX_LINE_LENGTH=300 -std=c99 -g -Wextra -pedantic
ngscmd_LDADD = $(LZ)
