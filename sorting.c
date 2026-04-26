//contains sorting
#include <stdlib.h>

typedef struct {
	int movie;
	double rating;
} movie_rating_pair;

static int cmp_pair_desc_rating(const void *a, const void *b){
	const movie_rating_pair *pa = (const movie_rating_pair *)a;
	const movie_rating_pair *pb = (const movie_rating_pair *)b;
	if (pa->rating < pb->rating) return 1;
	if (pa->rating > pb->rating) return -1;
	return 0;
}

void sort(int *recommended_movies, double *predicted_ratings, int no_of_recommended_movies){
	if(no_of_recommended_movies <= 1) return;

	movie_rating_pair *pairs = (movie_rating_pair *)malloc(sizeof(movie_rating_pair) * (size_t)no_of_recommended_movies);
	if(!pairs) return; // best-effort; leave unsorted on OOM

	for(int i=0;i<no_of_recommended_movies;i++){
		pairs[i].movie = recommended_movies[i];
		pairs[i].rating = predicted_ratings[i];
	}

	qsort(pairs, (size_t)no_of_recommended_movies, sizeof(movie_rating_pair), cmp_pair_desc_rating);

	for(int i=0;i<no_of_recommended_movies;i++){
		recommended_movies[i] = pairs[i].movie;
		predicted_ratings[i] = pairs[i].rating;
	}

	free(pairs);
}
