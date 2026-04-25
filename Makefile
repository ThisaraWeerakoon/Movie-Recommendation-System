CC ?= gcc
CFLAGS ?= -O2
CPPFLAGS ?=
LDFLAGS ?=
LDLIBS ?= -lm

SRC_COMMON = kmeans.c matrix_normalization.c pearsons.c predictions.c recommender.c sorting.c utility_matrix.c

.PHONY: all clean prof bench-grid

all: ui bench bench-grid

ui: ui.c $(SRC_COMMON)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ ui.c $(SRC_COMMON) $(LDFLAGS) $(LDLIBS)

bench: bench.c $(SRC_COMMON)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ bench.c $(SRC_COMMON) $(LDFLAGS) $(LDLIBS)

bench-grid: bench_grid.c $(SRC_COMMON)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ bench_grid.c $(SRC_COMMON) $(LDFLAGS) $(LDLIBS)

# Profiling-friendly build (keeps frames for perf/callgrind stacks)
prof:
	$(MAKE) clean
	$(MAKE) CFLAGS="-O2 -g -fno-omit-frame-pointer" all

clean:
	rm -f ui bench bench-grid a.out

