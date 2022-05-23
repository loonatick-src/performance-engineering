#include "coo_spmv_kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

// struct rInf {
//     int rowId;
//     int *cols;
//     int numOfCols;
// }
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
        // printf("A_rows %d -- A_cols %d, i :: %d :: A_vals: %lf\n", A_rows[i], A_cols[i], i, A_values[i]);
	    C[A_rows[i]] += A_values[i] * B[A_cols[i]];
    }
}

void coo_spmv_par(int nz, const int *A_rows, const int *A_cols, const float *A_values, const float *B, float *C) {
    int i;
    // float tmp;
    #pragma omp parallel for
    for (i = 0; i < nz; i++) {
        int idx_c = A_cols[i];
        int idx_r = A_rows[i];
	    float tmp = C[idx_r] +  A_values[i] * B[idx_c];
        C[idx_r] = tmp;
    }
    
}