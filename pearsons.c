// Centered cosine similarity (cosine of centered vectors; same kernel shape as before)
#include <math.h>
#include <immintrin.h>
#include <omp.h>

static inline double hsum256d(__m256d v)
{
	__m128d lo = _mm256_castpd256_pd128(v);
	__m128d hi = _mm256_extractf128_pd(v, 1);
	__m128d s = _mm_add_pd(lo, hi);
	return _mm_cvtsd_f64(s) + _mm_cvtsd_f64(_mm_unpackhi_pd(s, s));
}

double pearson_correlation(const double *A, const double *B, unsigned int size)
{
	__m256d vd = _mm256_setzero_pd();
	__m256d va = _mm256_setzero_pd();
	__m256d vb = _mm256_setzero_pd();
	unsigned int i = 0;
	const unsigned int n = size & ~3u;

	for (; i < n; i += 4) {
		__m256d a = _mm256_loadu_pd(A + i);
		__m256d b = _mm256_loadu_pd(B + i);
		vd = _mm256_fmadd_pd(a, b, vd);
		va = _mm256_fmadd_pd(a, a, va);
		vb = _mm256_fmadd_pd(b, b, vb);
	}

	double dot_p = hsum256d(vd);
	double mag_a = hsum256d(va);
	double mag_b = hsum256d(vb);

	for (; i < size; i++) {
		dot_p += A[i] * B[i];
		mag_a += A[i] * A[i];
		mag_b += B[i] * B[i];
	}

	/* One sqrt: sqrt(mag_a * mag_b) == sqrt(mag_a) * sqrt(mag_b) for nonnegative */
	return dot_p / sqrt(mag_a * mag_b);
}

void calc_similarity(double *normalizeduser, const double *normalized_matrix, double *similarity, int No_of_users, int No_of_movies)
{
#pragma omp parallel for schedule(static) if (No_of_users > 64)
	for (int i = 0; i < No_of_users; i++) {
		similarity[i] = pearson_correlation(
			normalizeduser,
			&normalized_matrix[(size_t)i * (size_t)No_of_movies],
			(unsigned int)No_of_movies);
	}
}
