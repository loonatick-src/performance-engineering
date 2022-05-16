#ifndef CSC_SPMV_KERNEL_H
#define CSC_SPMV_KERNEL_H

void csc_spmv(int n, const int *A_cols, const int *A_rows_idx, const float *A_values, const float *B, float *C);

void csc_spmv_par(int n, const int *A_cols, const int *A_rows_idx, const float *A_values, const float *B, float *C);

#endif


