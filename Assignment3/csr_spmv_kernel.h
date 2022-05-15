#ifndef CSR_SPMV_KERNEL_H
#define CSR_SPMV_KERNEL_H

void csr_spmv(int m, const int *A_rows, const int *A_cols_idx, const float *A_values, const float *B, float *C);

void csr_spmv_par(int m, const int *A_rows, const int *A_cols_idx, const float *A_values, const float *B, float *C, int max_threads);

#endif


