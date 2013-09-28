CC = gcc
AR = ar
PROG = NGSutils
LIB = libngs.a
LIB_SRC = fa2fq.c pair.c convert.c clean.c bypos.c sort.c revcom.c kmer.c
SRC = ngsutils.c
CFLAGS = -Wall -std=c99 -g -Wextra -pedantic
LIBFLAGS = -lz -lngs
LOBJS = $(LIB_SRC:%.c=%.o)
OBJS = $(SRC:%.c=%.o)

.SUFFIXES:.c .o
.PHONY all lib

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@

all : $(PROG)

$(PROG) : $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBFLAGS)

.PHONY : all lib clean

lib:libngs.a

libngs.a:$(LOBJS)
	$(AR) -csru $@ $(LOBJS)

clean :
	rm $(PROG) $(OBJS)
