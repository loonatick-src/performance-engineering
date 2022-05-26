#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <omp.h>
#include <time.h>

#define REP 10

int convert_to_csc(int m, int n, float *A, int *sA_cols, int *sA_row_idx, float *sA_vals) {
  int i,j;
  int checkNZ=0;
  float tmp;

  sA_cols[0]=0;
  checkNZ=0;
  for (i=0; i<n; i++) {
    for (j=0; j<m; j++) {
        tmp = A[j*n+i];
        if (tmp != 0) {
           sA_row_idx[checkNZ]=j;
           sA_vals[checkNZ]=tmp;
           checkNZ++;
        }
    }
    sA_cols[i+1]=checkNZ;
  }
  return checkNZ;
}

struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    return start_time;
}

long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}

void csr_spmv_par(int m, const int *A_rows, const int *A_cols_idx, const float *A_values, const float *B, float *C) {
    int i;
    #pragma omp parallel for
    for (i = 0; i < m; i++) {
        float tmp = 0.0f;
        for (int j =  A_rows[i]; j <  A_rows[i + 1]; j++) {
            int idx = A_cols_idx[j];
            tmp += A_values[j] * B[idx];
        }
        C[i] = tmp;
    }
}

void csc_spmv(int n, const int *A_cols, const int *A_rows_idx, const float *A_values, const float *B, float *C) {
    int i;
    printf("%d\n", n);
    int cc = 0;
    #pragma omp parallel for
    for (i = 0; i < n; i++) {
        float b_value = B[i];
        for (int j = A_cols[i]; j < A_cols[i + 1]; j++) {
            int ind = A_rows_idx[j];
            float temp = A_values[j] * b_value;
            #pragma omp atomic
            cc++;
            #pragma omp atomic
            C[ind] += temp;
	    }
    }
    printf("CC: %d\n", cc);
}

void print_mat(int m, int n, float *A) {
  int row, col; 

  for(row=0; row<m; row++) {
     for (col=0; col<n; col++) {
     	printf("%10.5f", A[row*n+col]);
     }
     printf("\n"); 
  }
}

void generate_mat(int m, int n, float *A, int nz) {
  int i;

  for (i=0; i<(m*n); i++) 
    A[i] = i ? i%10 == 0 : 0.0; //i/10; 
      	
}

int main (int argc, char** argv) {
    float *A, *B, *C;
    int m, n, nzA;

    float *sA_vals;
    int *sA_cols, *sA_rows_idx;
  int nums[] = {15, 128,256,384,512,640,768,896,1024,1152,1280,1408,1536,1664,1792,
  1920,2048,2176,2304,2432,2560,2688,2816,2944,3072,3200,3328,3456,
  3584,3712,3840,3968,4096,4224,4352,4480,4608,4736,4864,4992,5120,
  5248,5376,5504,5632,5760,5888,6016,6144,6272,6400,6528,6656,6784,
  6912,7040,7168,7296,7424,7552,7680,7808,7936,8064,8192,8320,8448,
  8576,8704,8832,8960,9088,9216,9344,9472,9600,9728,9856,9984};
  omp_set_num_threads(16);
   for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 5; j++) {
        m = nums[i];
        n = nums[j];
        nzA=m*n/10;
        printf("%d\n", nzA);

    A = (float *)calloc(m*n,sizeof(float));
    generate_mat(m,n,A,nzA);
    

    sA_cols = (int *)calloc(n+1,sizeof(int));
    sA_rows_idx = (int *)calloc(nzA,sizeof(int));
    sA_vals = (float *)calloc(nzA,sizeof(float));

    convert_to_csc(m,n, A, sA_cols, sA_rows_idx, sA_vals);
    // printf("Mark1\n");
    B = (float *)calloc(n,sizeof(float));
    generate_mat(n,1,B,1);
    C = (float *)calloc(m,sizeof(float));

    int r = 0;
    // printf("Mark1\n");
    struct timespec before ={0,0}, after={0,0};
    clock_gettime(CLOCK_MONOTONIC, &before); 

    for (r=0; r<REP; r++) {
        csc_spmv(n,sA_cols, sA_rows_idx, sA_vals, B, C);
    }
    clock_gettime(CLOCK_MONOTONIC, &after);
    double  time_elapsed_nanos = (((double)after.tv_sec*1.0e9 + after.tv_nsec) - ((double)before.tv_sec*1.0e9 + before.tv_nsec))/REP;

    printf("%d;%d;%d;%lf\n", m, n, nzA, time_elapsed_nanos); 

    free(sA_vals);
    free(sA_cols);
    free(sA_rows_idx);
    free(A);
    free(B);
    free(C);

     
    }
   }
    
    return 0;
}