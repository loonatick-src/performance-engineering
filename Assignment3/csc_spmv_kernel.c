#include "csc_spmv_kernel.h"
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
void csc_spmv(int n, const int *A_cols, const int *A_rows_idx, const float *A_values, const float *B, float *C) {
    int i,j;
    int col_start, col_end;  

    for (i = 0; i < n; i++) {
        col_start = A_cols[i];
        col_end = A_cols[i + 1];
        
        for (j = col_start; j < col_end; j++) {
            C[A_rows_idx[j]] += A_values[j] * B[i];
	}
    }
}

void csc_spmv_par(int n, const int *A_cols, const int *A_rows_idx, const float *A_values, const float *B, float *C, int max_threads) {
    int i;
    // int col_start, col_end;  
    
    if (floor(n / 2) < 128 || max_threads == 2) {
        omp_set_num_threads(2);
    } else if (floor(n / 4) < 128 || max_threads == 4) {
        omp_set_num_threads(4);
    } else if (floor(n / 8) < 128 || max_threads == 8) {
        omp_set_num_threads(8);
    } else {
        omp_set_num_threads(16);
    }
    printf("n:%d\n num_t: %d\n", n, omp_get_max_threads());
    #pragma omp parallel for
    for (i = 0; i < n; i++) {
        // col_start = ;
        // col_end = A_cols[i + 1];
        for (int j = A_cols[i]; j < A_cols[i + 1]; j++) {
            #pragma omp atomic
            C[A_rows_idx[j]] += A_values[j] * B[i];
	    }
    }
}
