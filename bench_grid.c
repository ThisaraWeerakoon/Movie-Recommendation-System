/*
 * Grid benchmark matching the repeats × UID-range table (wall-clock, monotonic).
 *
 * Loads the utility matrix and normalizes once, then for each table cell times only
 * the recommender loop (same work as the inner loops in bench.c). This isolates
 * scaling with repeats and UID count; CSV load is not repeated per cell.
 *
 * Run from the project directory (Dataset/ratings_learn.csv must resolve).
 *
 * Usage: ./bench-grid
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "recommender.h"
#include "utility_matrix.h"
#include "matrix_normalization.h"

static double monotonic_secs(const struct timespec *a, const struct timespec *b)
{
	return (double)(b->tv_sec - a->tv_sec) + (double)(b->tv_nsec - a->tv_nsec) * 1e-9;
}

static void run_cell(int start_uid, int end_uid, int repeats, int No_of_users,
		     double *utility_matrix, double *normalized_matrix,
		     double *out_secs)
{
	struct timespec t0, t1;
	if (clock_gettime(CLOCK_MONOTONIC, &t0) != 0) {
		perror("clock_gettime");
		exit(1);
	}
	for (int r = 0; r < repeats; r++) {
		for (int uid = start_uid; uid <= end_uid; uid++) {
			recommender_cached_quiet(uid, No_of_users, utility_matrix,
						 normalized_matrix);
		}
	}
	if (clock_gettime(CLOCK_MONOTONIC, &t1) != 0) {
		perror("clock_gettime");
		exit(1);
	}
	*out_secs = monotonic_secs(&t0, &t1);
}

int main(void)
{
	static const int repeats_rows[] = {10, 50, 90, 130, 170};
	static const int uid_ends[] = {50, 100, 200, 400};
	const int n_rows = (int)(sizeof repeats_rows / sizeof repeats_rows[0]);
	const int n_cols = (int)(sizeof uid_ends / sizeof uid_ends[0]);
	const int start_uid = 1;

	int No_of_users = findusers();
	const int No_of_movies = 9125;

	double *utility_matrix =
	    (double *)calloc((size_t)No_of_users * (size_t)No_of_movies, sizeof(double));
	double *normalized_matrix = (double *)malloc(
	    sizeof(double) * (size_t)No_of_users * (size_t)No_of_movies);
	if (!utility_matrix || !normalized_matrix) {
		fprintf(stderr, "Allocation failed.\n");
		free(utility_matrix);
		free(normalized_matrix);
		return 3;
	}

	fprintf(stderr, "bench_grid: loading CSV and normalizing (one-time)...\n");
	get_utility_matrix(utility_matrix, "Dataset/ratings_learn.csv", No_of_movies,
			   No_of_users, start_uid);
	normalize_matrix(utility_matrix, normalized_matrix, No_of_users, No_of_movies);

	/* TSV for spreadsheets */
	printf("Repeats");
	for (int c = 0; c < n_cols; c++) {
		printf("\tUIDs[1..%d]", uid_ends[c]);
	}
	printf("\n");

	for (int r = 0; r < n_rows; r++) {
		printf("%d", repeats_rows[r]);
		for (int c = 0; c < n_cols; c++) {
			double secs;
			run_cell(start_uid, uid_ends[c], repeats_rows[r], No_of_users,
				 utility_matrix, normalized_matrix, &secs);
			printf("\t%.3f", secs);
		}
		printf("\n");
	}

	fprintf(stderr,
		"bench_grid: done (times are wall seconds per cell, recommender work only).\n");

	free(utility_matrix);
	free(normalized_matrix);
	return 0;
}
