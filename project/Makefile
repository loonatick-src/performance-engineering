# I cannot for the life of my figure out how to change compiler flagst for a specific target in CMake -- CK

CC = g++
INCLUDE = -Isrc/common -I/src/theRestOfYourLife -Isrc/common/extra
CFLAGS = -Wall -Wextra -pedantic  -fopenmp -fwrapv $(INCLUDE) $(OPTFLAGS)


MAIN = src/TheRestOfYourLife/main.cc
TARGET = build/theRestOfYourLife

all: $(TARGET)

$(TARGET): build
	$(CC) $(CFLAGS) $(MAIN) -o $@

build:
	@mkdir -p build

clean:
	rm -rf build
