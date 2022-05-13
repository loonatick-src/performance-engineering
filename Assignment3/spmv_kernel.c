#include "spmv_kernel.h"


void spmv(int m, int n, float *A, float *B, float *C) {
   int i, j;
   float tmp;

   for(i=0; i<m; i++) {
      tmp=0;
      for(j=0; j<n; j++) {
         tmp += A[i*n+j]*B[j];
      }
      C[i]=tmp;	
   }
}
