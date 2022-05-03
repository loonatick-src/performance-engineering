#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void histogram(int * data, int n, int * bins, int k){
	int number_of_threads = omp_get_max_threads();

	int local_bins[number_of_threads][26];

	memset(bins, 0, sizeof(bins));
	memset(local_bins, 0, sizeof(local_bins));
	
	#pragma omp parallel num_threads(2)
	{
		int id = omp_get_thread_num();

		#pragma omp for
		for(int i = 0; i < n;i++)
		{
			local_bins[id][data[i] / k] ++;
		}
	}

	for(int i = 0;i < 26;i++)
	{
		for(int j = 0;j < number_of_threads;j++)
		{
			bins[i] += local_bins[j][i];
		}
	}
}

void naive_histogram(int * data, int n, int * bins, int k){
	memset(bins, 0, sizeof(bins));

	for(int i = 0;i < n;i++)
	{
		bins[data[i] / k] ++;
	}
}

int main()
{
	int n, k, *input, *bins, *bins_validate;

	n = 65536000;
	k = 10;

	srand(time(NULL));

	input = (int *)malloc(sizeof(int) * n);

	bins = (int *)malloc(sizeof(int) * 26);
	bins_validate = (int *)malloc(sizeof(int) * 26);

	for(int i = 0;i < n;i++)
	{
		input[i] = rand() % 256;
	}

	for(int j = 0; j < 26;j++)
	{
		bins[j] = 0;
		bins_validate[j] = 0;
	}	

	histogram(input, n, bins, k);
	naive_histogram(input, n, bins_validate, k);
	
	for(int k = 0;k < 26;k++)
	{
		printf("Evaluating bin %d with values %d and %d\n", k, bins[k], bins_validate[k]);
		if(bins[k] != bins_validate[k])
		{
			printf("Iteration %d has the wrong values: %d and %d\n", k, bins[k], bins_validate[k]);
			break;
		}
	}	

	return 0;
}
