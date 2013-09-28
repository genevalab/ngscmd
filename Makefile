CC = gcc
AR = ar
PROG = NGSutils
LIB_SRC = fa2fq.c fq2fa.c pair.c convert.c clean.c bypos.c sort.c revcom.c kmer.c index.c
SRC = ngsutils.c
CFLAGS = -Wall -std=c99 -g -Wextra -pedantic
LIBPATH = -L .
LIBFLAGS = -lz -lngs
LOBJS = $(LIB_SRC:%.c=%.o)
OBJS = $(SRC:%.c=%.o)

.SUFFIXES:.c .o

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

all : $(PROG)

$(PROG) : $(OBJS)
	$(CC) $(LIBPATH) -o $@ $(OBJS)  $(LIBFLAGS)

.PHONY : all lib clean

lib:libngs.a

libngs.a:$(LOBJS)
	$(AR) -csru $@ $(LOBJS)

clean :
	rm $(PROG) $(OBJS) $(LOBJS)
