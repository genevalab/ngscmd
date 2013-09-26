CC = gcc
PROG = NGSutils
SRC = ngsutils.c fa2fq.c pair.c convert.c clean.c bypos.c sort.c revcom.c
CFLAGS = -Wall -std=c99 -g -Wextra -pedantic
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
