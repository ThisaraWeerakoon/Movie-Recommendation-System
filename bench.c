#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "recommender.h"
#include "utility_matrix.h"
#include "matrix_normalization.h"

// Usage:
//   ./bench <start_uid> <end_uid> [repeats]
// Example:
//   ./bench 1 500 1
int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <start_uid> <end_uid> [repeats]\n", argv[0]);
        return 2;
    }

    int start_uid = atoi(argv[1]);
    int end_uid = atoi(argv[2]);
    int repeats = (argc >= 4) ? atoi(argv[3]) : 1;

    if (start_uid <= 0 || end_uid < start_uid || repeats <= 0) {
        fprintf(stderr, "Invalid args.\n");
        return 2;
    }

    struct timespec wall0, wall1;
    if (clock_gettime(CLOCK_MONOTONIC, &wall0) != 0) {
        perror("clock_gettime");
        return 1;
    }

    int No_of_users = findusers();
    const int No_of_movies = 9125;

    double *utility_matrix = (double *)calloc((size_t)No_of_users * (size_t)No_of_movies, sizeof(double));
    double *normalized_matrix = (double *)malloc(sizeof(double) * (size_t)No_of_users * (size_t)No_of_movies);
    if(!utility_matrix || !normalized_matrix){
        fprintf(stderr, "Allocation failed.\n");
        free(utility_matrix);
        free(normalized_matrix);
        return 3;
    }

    get_utility_matrix(
        utility_matrix,
        "Dataset/ratings_learn.csv",
        No_of_movies,
        No_of_users,
        start_uid
    );
    normalize_matrix(utility_matrix, normalized_matrix, No_of_users, No_of_movies);

    for (int r = 0; r < repeats; r++) {
        for (int uid = start_uid; uid <= end_uid; uid++) {
            // Same algorithm as UI path, but avoids interactive printing and extra IO.
            recommender_cached_quiet(uid, No_of_users, utility_matrix, normalized_matrix);
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &wall1) != 0) {
        perror("clock_gettime");
        free(utility_matrix);
        free(normalized_matrix);
        return 1;
    }
    /* Wall time: clock() sums CPU time across threads and is misleading with OpenMP. */
    double secs = (double)(wall1.tv_sec - wall0.tv_sec)
        + (double)(wall1.tv_nsec - wall0.tv_nsec) * 1e-9;
    printf("bench: uids=[%d..%d] repeats=%d elapsed_wall=%.3f seconds\n",
           start_uid, end_uid, repeats, secs);

    free(utility_matrix);
    free(normalized_matrix);
    return 0;
}

