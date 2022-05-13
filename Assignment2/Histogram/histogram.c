#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void histogram(int * data, int n, int * bins, int k){
	memset(bins, 0, sizeof(bins));
	
	#pragma omp parallel num_threads(2)
	{
		#pragma omp for
		for(int i = 0; i < n;i++)
		{
			#pragma omp atomic
			bins[data[i] / k] ++;
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
