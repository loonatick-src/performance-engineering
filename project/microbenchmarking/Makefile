CC = g++
INCLUDE = 
CFLAGS = -O3 -Wall -Wextra -pedantic -fopenmp -fwrapv $(INCLUDE) $(OPTFLAGS)


all: rand shared_ptr

rand: rand.cc
	$(CC) $(CFLAGS) rand.cc -o $@

shared_ptr: shared_ptr.cc
	$(CC) $(CFLAGS) shared_ptr.cc -o $@

clean:
	rm -f rand shared_ptr
