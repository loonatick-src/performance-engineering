#ifndef COO_SPMV_KERNEL_H
#define COO_SPMV_KERNEL_H

void coo_spmv(int nz, const int *A_rows, const int *A_cols, const float *A_values, const float *B, float *C);

void coo_spmv_par(int nz, const int *A_rows, const int *A_cols, const float *A_values, const float *B, float *C, int max_threads);

#endif


