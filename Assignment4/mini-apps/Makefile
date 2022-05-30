CC = gcc

CFLAGS = -Wall -Wextra -O2 -fopenmp -I$(PAPI_HOME)/include $(OPTFLAGS)
LDLIBS = -L$(PAPI_HOME)/lib -lpapi -lpfm  # TODO: is libpfm.a required?

SRC = $(wildcard *.c)
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

papi:
	echo $(PAPI_HOME)
