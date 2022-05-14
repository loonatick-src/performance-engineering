#include "coo_spmv_kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
/*
 * m = number of cols 
 * A_cols = col pointers, m+1 of them, each element with the start of the nonZero's in the columns array 
 * A_cols_idx = column indices of the nonZero values  
 * A_values = the actual values in the nonZero locations 
 * B = the vector 
 * C = the result 
 */
void coo_spmv(int nz, const int *A_rows, const int *A_cols, const float *A_values, const float *B, float *C) {
    int i;

    for (i = 0; i < nz; i++) {
	C[A_rows[i]] += A_values[i] * B[A_cols[i]];
    }
}

void coo_spmv_par(int nz, const int *A_rows, const int *A_cols, const float *A_values, const float *B, float *C) {
    int i;
    // Idear: spread work out over num of threads and then merge them later.
    // It means to malloc a new array
    int tNum = omp_get_max_threads();
    int amount_of_loop = floor(nz / tNum);
    int lastSize = nz - (amount_of_loop * tNum);
    float newC[] = {};

    #pragma omp parallel 
    {
        float *fakeC;
        bool isLast = (omp_get_thread_num() == tNum);
        if (isLast) {
            fakeC = (float *)calloc(lastSize, sizeof(float));
        } else {
            fakeC = (float *)calloc(amount_of_loop, sizeof(float));
        }
        int startVal = amount_of_loop * omp_get_thread_num();
        int endVal = amount_of_loop * ( 1 + omp_get_thread_num()) ? !isLast : lastSize;
        for (i = startVal; i < endVal; i++) {
	        fakeC[A_rows[i]] += A_values[i] * B[A_cols[i]];
        }
        //todo: merge back

    }
    
}

