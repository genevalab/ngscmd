CC= gcc
AR= ar
PROG= NGSutils
LIB= libngs
LIB_SRC = fa2fq.c fq2fa.c pair.c convert.c clean.c bypos.c sort.c revcom.c kmer.c index.c count.c
SRC = ngsutils.c
CFLAGS = -Wall -std=c99 -g -Wextra -pedantic
LIBPATH = -L.
LIBFLAGS = -lz -lngs
LOBJS = $(LIB_SRC:%.c=%.o)
OBJS = $(SRC:%.c=%.o)

all: $(LIB) $(PROG)

$(LIB): $(LOBJS)
	$(AR) -csru libngs.a $(LOBJS)

$(PROG): $(OBJS)
	$(CC) $(LIBPATH) -o $@ $(OBJS)  $(LIBFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean :
	rm $(PROG) $(OBJS) libngs.a $(LOBJS)

