CC?= gcc
PROG= ngscmd
SRC = ngscmd.c makedb.c sort.c pair.c score.c format.c clean.c rmdup.c kmer.c
CFLAGS = -Wall -D_BSD_SOURCE -std=c99 -g -Wextra -pedantic
LIBFLAGS = -lz -ldb
OBJS = $(SRC:%.c=%.o)

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) -o $@ $(OBJS)  $(LIBFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	rm $(PROG) $(OBJS)

