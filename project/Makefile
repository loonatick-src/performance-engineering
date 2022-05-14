CC = g++
INCLUDE = -Isrc/TheRestOfYourLife/ -Isrc/common/ -Isrc/common/extra/
CFLAGS = -O3 -fopenmp -fwrapv $(INCLUDE) $(OPTFLAGS)
CFLAGS_INIT = -fopenmp -Wall -Wextra -pedantic -fwrapv $(INCLUDE) $(OPTFLAGS)


MAIN = src/TheRestOfYourLife/main.cc
TARGET = build/theRestOfYourLife

all: $(TARGET)

release: CFLAGS += -DNDEBUG
release: $(TARGET)

profile_gprof: CFLAGS += -pg
profile_gprof: profile

profile: CFLAGS += -fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
profile: release

flat_profile_gprof: CFLAGS += -pg
flat_profile_gprof: flat_profile

flat_profile: CFLAGS += -g
flat_profile: release

debug: CFLAGS = -g -Wall -Wextra -pedantic -fopenmp -fwrapv $(INCLUDE) $(OPTFLAGS)
debug: $(TARGET)

$(TARGET): src/TheRestOfYourLife/main.cc src/TheRestOfYourLife/*.h src/common/*.h build
	$(CC) $(CFLAGS) $(MAIN) -o $@

bench: bench2/rect_hit.cpp build
	$(CC) -g $(CFLAGS) -mno-omit-leaf-frame-pointer -fno-omit-frame-pointer bench2/*.cpp -o build/bench

build:
	@mkdir -p build

clean:
	rm -rf build
