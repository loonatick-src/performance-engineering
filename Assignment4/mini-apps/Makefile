CC = gcc

CFLAGS = -std=gnu11 -Wall -Wextra -O2 -fopenmp -DLIKWID_PERFMON -I$(LIKWID_HOME)/include $(OPTFLAGS)
LDLIBS = -L$(LIKWID_HOME)/lib -llikwid

# SRC = $(wildcard *.c)
SRC = false_sharing.c
TARGETS = $(patsubst %.c, build/%.out, $(SRC))

all: build $(TARGETS)

build/%.out: %.c
	$(CC) $(CFLAGS) $< -o $@ -lm $(LDLIBS)

src:
	@echo $(SRC)

build:
	@mkdir -p build

clean:
	@rm -rf build
