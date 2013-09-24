CC = gcc
PROG = NGSutils
SRC = ngsutils.c getopt.c fa2fq.c pair.c convert.c 
CFLAGS = -Wall -ansi -g
LIBFLAGS = -lz
OBJS = $(SRC:%.c=%.o)

all : $(PROG)

%.o : %.c
	$(CXX) $(CFLAGS) -o $@ -c $<

$(PROG) : $(OBJS)
	$(LD) -o $@ $(OBJS) $(LIBFLAGS)

.PHONY : clean

clean :
	rm $(PROG) $(OBJS)
