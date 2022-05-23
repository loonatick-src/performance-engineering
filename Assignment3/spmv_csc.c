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

void csc_spmv(int n, const int *A_cols, const int *A_rows_idx, const float *A_values, const float *B, float *C) {
    int i;
    #pragma omp parallel for
    for (i = 0; i < n; i++) {
        float b_value = B[i];
        for (int j = A_cols[i]; j < A_cols[i + 1]; j++) {
            int ind = A_rows_idx[j];
            float temp = A_values[j] * b_value;
            #pragma omp atomic
            C[ind] += temp;
	    }
    }
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
	A[i] = i ? i%nz == 0 : 0.0; //i/10; 
      	
}

int main (int argc, char** argv) {
    float *A, *B, *C;
    int m, n, nzA;

    float *sA_vals;
    int *sA_cols, *sA_rows_idx;

    omp_set_num_threads(16);
    srand(time(NULL));


    m = rand() % 1024;
    n = rand() % 1024;
    nzA=m*n/10;

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
    struct timespec vartime = timer_start();

    for (r=0; r<REP; r++) {
        csc_spmv(n,sA_cols, sA_rows_idx, sA_vals, B, C);
    }
    

    long time_elapsed_nanos = timer_end(vartime); 

    printf("Time taken by kernel for matrixes of %d by %d and Non-zero: %d: %ld\n", m, n, nzA, time_elapsed_nanos/10); 

    free(sA_vals);
    free(sA_cols);
    free(sA_rows_idx);
    free(A);
    free(B);
    free(C);

    return 0;
}