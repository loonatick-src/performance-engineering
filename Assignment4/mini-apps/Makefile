CC = gcc

CFLAGS = -std=c11 -Wall -Wextra -O2 -fopenmp -DLIKWID_PERFMON -I$(LIKWID_HOME)/include $(OPTFLAGS)
LDLIBS = -L$(LIKWID_HOME)/lib -llikwid# -lpapi -lpfm  # TODO: is libpfm.a required?

# SRC = $(wildcard *.c)
SRC = load_balance.c
TARGETS = $(patsubst %.c, build/%.out, $(SRC))

all: build $(TARGETS)

build/%.out: %.c
	$(CC) $(CFLAGS) $< -o $@ -lm #$(LDLIBS)

src:
	@echo $(SRC)

build:
	@mkdir -p build

clean:
	@rm -rf build
