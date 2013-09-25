CC = gcc
PROG = NGSutils
SRC = ngsutils.c getopt.c fa2fq.c pair.c convert.c clean.c bypos.c sort.c
CFLAGS = -Wall -ansi -g
LIBFLAGS = -lz
OBJS = $(SRC:%.c=%.o)

all : $(PROG)

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(PROG) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBFLAGS)

.PHONY : clean

clean :
	rm $(PROG) $(OBJS)
