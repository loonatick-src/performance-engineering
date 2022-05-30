CC = gcc

CFLAGS = -Wall -Wextra -O2 -I$(PAPI_HOME)/include $(OPTFLAGS)
LDLIBS = -L$(PAPI_HOME)/lib -lpapi -lpfm  # TODO: is libpfm.a required?

SRC = $(wildcard %.c)
TARGETS = $(patsubst %.c, %.out, $(SRC))

all: build $(TARGETS)

%.out: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

build:
	@mkdir -p build

clean:
	@rm -rf build

papi:
	echo $(PAPI_HOME)
