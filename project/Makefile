CC = g++
INCLUDE = -Isrc/TheRestOfYourLife/ -Isrc/common/ -Isrc/common/extra/
CFLAGS = -O3 -Wall -Wextra -pedantic  -fopenmp -fwrapv $(INCLUDE) $(OPTFLAGS)
CFLAGS_INIT = -fopenmp -Wall -Wextra -pedantic -fwrapv $(INCLUDE) $(OPTFLAGS)
BENCH_DIR = bench

# SRC_GBENCH = $(wildcard bench/*_bench.cpp)
# TARGET_GBENCH = $(patsubst bench/%.cpp, build/%, $(SRC_GBENCH))

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


SRC_BENCH = $(wildcard bench/*_bench.cpp)
TARGET_BENCH = $(patsubst bench/%.cpp, build/%, $(SRC_BENCH))

GOOGLE_BENCHMARK_INCLUDE=-Ibenchmark/install/include/
LD_GOOGLE_BENCHMARK=-Lbenchmark/install/lib64/

bench: build $(TARGET_BENCH)

build/%_bench: bench/%_bench.cpp
	$(CC) -g $(CFLAGS) $(GOOGLE_BENCHMARK_INCLUDE) -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer $< -o $@ $(LD_GOOGLE_BENCHMARK) -lbenchmark

build:
	@mkdir -p build

clean:
	rm -rf build
