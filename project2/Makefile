OMPFLAGS = -fopenmp
OMPLIBS = -lgomp

CC = gcc
CFLAGS = -g -Wall -DLEVEL1_DCACHE_LINESIZE=`getconf LEVEL1_DCACHE_LINESIZE` $(OMPFLAGS)
LDLIBS = $(OMPLIBS)

barrier_test: gtmp_counter.c barrier_test.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o *.dSYM barrier_test