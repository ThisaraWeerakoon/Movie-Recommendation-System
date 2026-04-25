CC ?= gcc
CFLAGS ?= -O3 -march=native -fopenmp
CPPFLAGS ?=
LDFLAGS ?=
LDLIBS ?= -lm

SRC_COMMON = kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c

.PHONY: all clean prof perf-bench bench-grid

all: ui bench bench-grid

bench-grid: bench_grid.c $(SRC_COMMON)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ bench_grid.c $(SRC_COMMON) $(LDFLAGS) $(LDLIBS)

ui: ui.c $(SRC_COMMON)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ ui.c $(SRC_COMMON) $(LDFLAGS) $(LDLIBS)

bench: bench.c $(SRC_COMMON)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ bench.c $(SRC_COMMON) $(LDFLAGS) $(LDLIBS)

# Profiling-friendly build (keeps frames for perf/callgrind stacks)
prof:
	$(MAKE) clean
	$(MAKE) CFLAGS="-O2 -g -fno-omit-frame-pointer -fopenmp -march=native" all

# Sample perf run: cycles, IPC, cache hierarchy (adjust uids to match your dataset)
perf-bench: bench
	perf stat -e cycles,instructions,cache-references,cache-misses,LLC-loads,LLC-load-misses -d ./bench 1 100 2

clean:
	rm -f ui bench bench-grid a.out

