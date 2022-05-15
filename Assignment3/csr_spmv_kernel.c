#include "csr_spmv_kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
/*
 * m = number of rows 
 * A_rows = row pointers, m+1 of them, each element with the start of the nonZero's in the columns array 
 * A_cols_idx = column indices of the nonZero values  
 * A_values = the actual values in the nonZero locations 
 * B = the vector 
 * C = the result 
 */

void csr_spmv(int m, const int *A_rows, const int *A_cols_idx, const float *A_values, const float *B, float *C) {
    int i,j;
    int row_start, row_end;  

    for (i = 0; i < m; i++) {
        float tmp = 0.0f;
        row_start = A_rows[i];
        row_end = A_rows[i + 1];
        // printf("What the A? %d :: %d\n", A_rows[i],A_rows[i + 1]);
        
        for (j = row_start; j < row_end; j++) {
            tmp += A_values[j] * B[A_cols_idx[j]];
        }
        
        C[i] = tmp;
    }
}

void csr_spmv_par(int m, const int *A_rows, const int *A_cols_idx, const float *A_values, const float *B, float *C, int max_threads) {
    int i;
    // int row_start, row_end; 
    // float tmp; 
    // row_start = A_rows[i];
    // row_end = A_rows[i + 1];
    // printf("max:%d\n", omp_get_max_threads());
    if (floor(m / 2) < 128 || max_threads == 2) {
        omp_set_num_threads(2);
    } else if (floor(m / 4) < 128 || max_threads == 4) {
        omp_set_num_threads(4);
    } else if (floor(m / 8) < 128 || max_threads == 8) {
        omp_set_num_threads(8);
    } else {
        omp_set_num_threads(16);
    }
    #pragma omp parallel for     
    for (i = 0; i < m; i++) {
        // tmp = 0.0f;
        float tmp = 0.0f;
        // row_start = A_rows[i];
        // row_end = A_rows[i + 1];
        
        // printf("What the A? %d :: %d\n", A_rows[i],A_rows[i + 1]);
        // #pragma omp parallel for reduction( + : tmp)
        for (int j =  A_rows[i]; j <  A_rows[i + 1]; j++) {
            tmp += A_values[j] * B[A_cols_idx[j]];
        }
        
        // #pragma omp atomic
        // #pragma omp atomic
        C[i] = tmp;
        
        // row_start = row_end;
    }
   
}
