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

void coo_spmv_par(int nz, const int *A_rows, const int *A_cols, const float *A_values, const float *B, float *C, int max_threads) {
    // int i;
    // int *sCols;
     if (floor(nz / 2) < 128 || max_threads == 2) {
        omp_set_num_threads(2);
    } else if (floor(nz / 4) < 128 || max_threads == 4) {
        omp_set_num_threads(4);
    } else if (floor(nz / 8) < 128 || max_threads == 8) {
        omp_set_num_threads(8);
    } else {
        omp_set_num_threads(16);
    }
    // // #pragma omp parallel for schedule(dynamic) 
    // int *howMany = (int *) calloc(nz, sizeof(int));
    // #pragma omp parallel for reduction
    // for (int b=0;b<nz;b++) {
    //     howMany[A_rows[b]]++;
    // }
    int i;
    #pragma omp parallel for
    for (i = 0; i < nz; i++) {
        // printf("A_rows %d -- A_cols %d, i :: %d :: A_vals: %lf\n", A_rows[i], A_cols[i], i, A_values[i]);
        #pragma omp atomic
	    C[A_rows[i]] += A_values[i] * B[A_cols[i]];
    }
    // for (i = 0; i < nz; i++) {
    //     printf("A_rows %d -- A_cols %d, i :: %d\n", A_rows[i], A_cols[i], i);
    //     // #pragma omp atomic
    //     float tmp = 0.0f;
    //     // if (howMany[A_rows[i]] == 2) {
    //     //     C[A_rows[i]] += A_values[i] * B[A_cols[i]] + A_values[i + 1] * B[A_cols[i + 1]];
    //     // } else {
    //     if (howMany[A_rows[i]] == 1) {
    //         #pragma omp atomic
    //         C[A_rows[i]] += A_values[i] * B[A_cols[i]];
    //         continue;
    //     }
    //     for (int p = 0; p < howMany[A_rows[i]]; p++) {
    //         tmp += A_values[i + p] * B[A_cols[i + p]];
    //     }
    //     #pragma omp atomic
    //     C[A_rows[i]] += tmp;
        
    //     // }
	    
        
    // }
    
}

