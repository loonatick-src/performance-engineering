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
        // printf("i:%d\ncols: %d\ncole:%d\n", i, col_start,col_end);
        for (j = col_start; j < col_end; j++) {
            // printf("All i :: j's:%d :: %d\n", i, j);
            
            C[A_rows_idx[j]] += A_values[j] * B[i];
	}
    }
}

void csc_spmv_par(int n, const int *A_cols, const int *A_rows_idx, const float *A_values, const float *B, float *C) {
    int i;
    #pragma omp parallel for schedule(static)
    for (i = 0; i < n; i++) {
        for (int j = A_cols[i]; j < A_cols[i + 1]; j++) {
            int ind = A_rows_idx[j];
            float temp = A_values[j] * B[i];
            #pragma omp atomic
            C[ind] += temp;
             
	    }
    }
}
