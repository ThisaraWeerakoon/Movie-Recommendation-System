//header file for recommender
#ifndef RECOMMENDER_H
#define RECOMMENDER_H

void recommender(
				int uid
				);

// Batch/benchmark entrypoint: same algorithm, reduced printing/IO.
void recommender_quiet(
				int uid
				);

// Returns max user id found in ratings_learn.csv
int findusers(void);

// Batch-optimized path: reuse precomputed matrices.
void recommender_cached_quiet(
				int uid,
				int No_of_users,
				const double *utility_matrix,
				const double *normalized_matrix
				);
				
#endif
