#include "csr_spmv_kernel.h"
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
        
        for (j = row_start; j < row_end; j++) {
            tmp += A_values[j] * B[A_cols_idx[j]];
        }
        
        C[i] = tmp;
    }
}

void csr_spmv_par(int m, const int *A_rows, const int *A_cols_idx, const float *A_values, const float *B, float *C) {
    int i,j;
    int row_start, row_end;  
    #pragma omp parallel for
    for (i = 0; i < m; i++) {
        float tmp = 0.0f;
        row_start = A_rows[i];
        row_end = A_rows[i + 1];
        // #pragma omp parallel for reduction(tmp:+)
        for (j = row_start; j < row_end; j++) {
            tmp += A_values[j] * B[A_cols_idx[j]];
        }
        
        C[i] = tmp;
    }
}
