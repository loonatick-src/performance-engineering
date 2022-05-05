# I cannot for the life of my figure out how to change compiler flags for a specific target in CMake -- CK

CC = g++
INCLUDE = -Isrc/common -I/src/theRestOfYourLife -Isrc/common/extra
CFLAGS = -O3 -Wall -Wextra -pedantic  -fopenmp -fwrapv $(INCLUDE) $(OPTFLAGS)
CFLAGS_INIT = -Wall -Wextra -pedantic -fwrapv $(INCLUDE)


MAIN = src/TheRestOfYourLife/main.cc
TARGET = build/theRestOfYourLife

all: $(TARGET)

profile: CFLAGS = -pg $(CFLAGS_INIT) -O3
profile: release

release: CFLAGS += -DNDEBUG
release: $(TARGET)

debug: CFLAGS = -g -Wall -Wextra -pedantic -fopenmp -fwrapv $(INCLUDE) $(OPTFLAGS)
debug: $(TARGET)

$(TARGET): build src/TheRestOfYourLife/*.cc src/common/*.h
	$(CC) $(CFLAGS) $(MAIN) -o $@

build:
	@mkdir -p build

clean:
	rm -rf build
