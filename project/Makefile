CC = g++
INCLUDE = -Isrc/TheRestOfYourLife/ -Isrc/common/ -Isrc/common/extra/
CFLAGS = -O3 -Wall -Wextra -pedantic  -fopenmp -fwrapv $(INCLUDE) $(OPTFLAGS)
CFLAGS_INIT = -fopenmp -Wall -Wextra -pedantic -fwrapv $(INCLUDE) $(OPTFLAGS)

SRC_BENCH = $(wildcard bench2/*.cpp)
TARGET_BENCH = $(patsubst bench2/%.cpp, build/%, $(SRC_BENCH))

MAIN = src/TheRestOfYourLife/main.cc
TARGET = build/theRestOfYourLife

all: $(TARGET)

release: CFLAGS += -DNDEBUG
release: $(TARGET)

profile_gprof: CFLAGS += -pg
profile_gprof: release

profile_perf: CFLAGS += -fno-omit-frame-pointer -mno-omit-frame-pointer
profile_perf: release


debug: CFLAGS = -g -Wall -Wextra -pedantic -fopenmp -fwrapv $(INCLUDE) $(OPTFLAGS)
debug: $(TARGET)

$(TARGET): build src/TheRestOfYourLife/main.cc src/TheRestOfYourLife/*.h src/common/*.h
	$(CC) $(CFLAGS) $(MAIN) -o $@

# bench: bench2/rect_hit.cpp build
#	$(CC) -g $(CFLAGS) -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer bench2/*.cpp -o build/bench

bench: $(TARGET_BENCH)

build/%_bench: bench2/%_bench.cpp build
	$(CC) -g $(CFLAGS) -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer $< -o $@ #-lbenchmark -lpfm

build:
	@mkdir -p build

clean:
	rm -rf build
