/* SpMV: this file contains the I/O functions used to read and write matrices
 * in the Market Matrix format(see
 * https://math.nist.gov/MatrixMarket/formats.html#MMformat), using the
 * functions in mmio.c in its turn.
 * A vector of the appropriate size is generated and multiplied with the matrix.
 * There are also functions to generate your own matrices.
 * The reading also supports the "is_pattern" flag from the Matrix Market format. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mmio.h"

#ifdef CSR
#include "csr_spmv_kernel.h"
#elif CSC
#include "csc_spmv_kernel.h"
#elif COO
#include "coo_spmv_kernel.h"
#else
#include "spmv_kernel.h"
#endif

//#define VERBOSE

#define N  512
#define M  512

#define REP 1


/* 
 * for nz=1 - dense matrix; for nz>1 - sparse matrix with every nz element non-0; for nz=0: error
 */
void generate_mat(int m, int n, float *A, int nz) {
  int i;

  for (i=0; i<(m*n); i++) 
	A[i] = i/nz; //i/10; 
      	
}

void read_sparse(FILE *f, int m, int n, int nz, float *A, int is_pattern) {
  int i, row, col;
  float val;  
 
    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    if (is_pattern) {
        for (i=0; i<nz; i++)
        {
            fscanf(f, "%d %d\n", &row, &col);
            A[(row-1)*n+col-1] = 1.0f;   /* adjust from 1-based to 0-based */
        }
    }
    else {
      for (i=0; i<nz; i++)
      {
          fscanf(f, "%d %d %f\n", &row, &col, &val);
          A[(row-1)*n+col-1] = val;   /* adjust from 1-based to 0-based */
      }
    }
}

void write_sparse(FILE *f, int m, int p, const float *C) {
   int i, nz=0; 
   MM_typecode matcode;

   for (i=0; i<m*p; i++) if (C[i] != 0.0) nz++; 

    mm_initialize_typecode(&matcode);
    mm_set_matrix(&matcode);
    mm_set_coordinate(&matcode);
    mm_set_real(&matcode);

    mm_write_banner(f, matcode); 
    mm_write_mtx_crd_size(f, m, p, nz);

    for (i=0; i<m*p; i++) {
	if (C[i] != 0.0) 
          fprintf(f, "%d %d %f\n", i/p+1, i%p+1, C[i]);
    }

}

void write_vector(FILE* f, int m, const float* C) {
  int i;

  for (i=0; i<m; i++) {
      fprintf(f, "%f\n", C[i]);
  }
}

void read_dense(FILE *f, int m, int n, float *A) {
  int row, col;

  for(row=0; row<m; row++) { 
     for (col=0; col<n; col++) {
        fscanf(f, "%f ", &A[row*n+col]); 
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


int read_mat(int *m, int *n, int *nzA, FILE* fa, int *is_pattern) {
  MM_typecode ta;
  int ret_code; 

  if (mm_read_banner(fa, &ta) != 0)
    {
        printf("Could not process Matrix Market banner for A.\n");
        return -3;
    }

  if (mm_is_pattern(ta)) 
	*is_pattern = 1;
  else
	*is_pattern = 0;


  if (mm_is_complex(ta)) return -6;

  if (mm_is_matrix(ta) && mm_is_sparse(ta))
    {
        if ((ret_code = mm_read_mtx_crd_size(fa, m, n, nzA)) !=0)
           return -10;
    }
  else if (mm_is_matrix(ta) && mm_is_array(ta)) {
	*nzA = 0;
        if ((ret_code = mm_read_mtx_array_size(fa, m, n)) !=0)
           return -11;

    }
  else return -8; 

  return 0;
}

#ifdef CSR
/*
 * Converts matrix to CSR format. 
 * returns the number of nonZero's found during conversion
 */
int convert_to_csr(int m, int n, float *A, int *sA_rows, int *sA_col_idx, float *sA_vals) {
  int i,j; 
  int checkNZ=0; 
  float tmp;
 
  sA_rows[0]=0;
  checkNZ=0;
  for (i=0; i<m; i++) {
    for (j=0; j<n; j++) {
	tmp = A[i*n+j];
	if (tmp != 0) {
 	   sA_col_idx[checkNZ]=j;
	   sA_vals[checkNZ]=tmp;
	   checkNZ++; 
	}	
    }	
    sA_rows[i+1]=checkNZ;
  }
  return checkNZ;
}

void print_mat_csr(int m, int nzA, int *sA_rows, int *sA_col_idx, float *sA_vals)
{
  int i;
  for (i=0; i<m+1; i++)
    printf("%d ", sA_rows[i]);
  printf("\n");

  for (i=0; i<nzA; i++)
    printf("%d ", sA_col_idx[i]);

  printf("\n");

  for (i=0; i<nzA; i++)
    printf("%10.5f ", sA_vals[i]);
  printf("\n");

}

#endif

#ifdef CSC
/*
 *  * Converts matrix to CSC format. 
 *   * returns the number of nonZero's found during conversion
 *    */
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

void print_mat_csc(int n, int nzA, int *sA_cols, int *sA_row_idx, float *sA_vals)
{
  int i;
  for (i=0; i<n+1; i++)
    printf("%d ", sA_cols[i]);
  printf("\n");

  for (i=0; i<nzA; i++)
    printf("%d ", sA_row_idx[i]);

  printf("\n");

  for (i=0; i<nzA; i++)
    printf("%10.5f ", sA_vals[i]);
  printf("\n");

}

#endif

#ifdef COO
/*
 *  * Converts matrix to CSC format. 
 *   *   * returns the number of nonZero's found during conversion
 *    *    */
int convert_to_coo(int m, int n, float *A, int *sA_rows, int *sA_cols, float *sA_vals) {
  int i,j;
  int checkNZ=0;
  float tmp;

  checkNZ=0;
  for (i=0; i<m; i++) {
    for (j=0; j<n; j++) {
        tmp = A[i*n+j];
        if (tmp != 0) {
           sA_rows[checkNZ]=i;
	   sA_cols[checkNZ]=j;
           sA_vals[checkNZ]=tmp;
           checkNZ++;
        }
    }
  }
  return checkNZ;
}

void print_mat_coo(int nzA, int *sA_rows, int *sA_cols, float *sA_vals)
{
  int i;
  for (i=0; i<nzA; i++)
    printf("%d ", sA_rows[i]);
  printf("\n");

  for (i=0; i<nzA; i++)
    printf("%d ", sA_cols[i]);

  printf("\n");

  for (i=0; i<nzA; i++)
    printf("%10.5f ", sA_vals[i]);
  printf("\n");

}

#endif




int main (int argc, char** argv) {
 float *A, *B, *C;
#ifdef CSR
 float *sA_vals;
 int *sA_rows, *sA_cols_idx;
#elif CSC
 float *sA_vals;
 int *sA_cols, *sA_rows_idx;
#elif COO
 float *sA_vals;
 int *sA_cols, *sA_rows;
#endif



 struct timeval before, after;
 int r, m, n, err;
 int nzA=0, is_pattern = 1;
 FILE *fa, *fc;
  
#ifdef GENERATE 
 m=M; n=N; nzA=M*N/10; 
#else 
 if (argc < 3) {
    fprintf(stderr, "Usage: %s [matrix-market-filename] [result-vector-filename]\n", argv[0]);
    exit(1);
 }
 else {
    if ((fa = fopen(argv[1], "rt")) == NULL) exit(1);
    err = read_mat(&m, &n, &nzA, fa, &is_pattern);    
    if (err == -15) {
	printf("Matrices are incompatible! \n");
	fclose(fa); 
	exit(1);
    }
 }
#endif

 A = (float *)calloc(m*n,sizeof(float));
 if (A==NULL) {printf("Out of memory A! \n"); exit(1);}

#ifdef GENERATE
   generate_mat(m,n,A,nzA);
#else 
   if (nzA>0) {
	read_sparse(fa, m,n,nzA, A, is_pattern);
    }	
   else 
	read_dense(fa, m,n, A);
   fclose(fa);

#ifdef CSR
        sA_rows = (int *)calloc(m+1,sizeof(int));
        sA_cols_idx = (int *)calloc(nzA,sizeof(int));
        sA_vals = (float *)calloc(nzA,sizeof(float));

        convert_to_csr(m,n, A, sA_rows, sA_cols_idx, sA_vals);
#ifdef VERBOSE
	print_mat(m,n,A);	
 	print_mat_csr(m, nzA, sA_rows, sA_cols_idx, sA_vals);
#endif

#elif CSC
        sA_cols = (int *)calloc(n+1,sizeof(int));
        sA_rows_idx = (int *)calloc(nzA,sizeof(int));
        sA_vals = (float *)calloc(nzA,sizeof(float));

        convert_to_csc(m,n, A, sA_cols, sA_rows_idx, sA_vals);
#ifdef VERBOSE
        print_mat(m,n,A);
        print_mat_csc(m, nzA, sA_cols, sA_rows_idx, sA_vals);
#endif

#elif COO
        sA_cols = (int *)calloc(nzA,sizeof(int));
        sA_rows = (int *)calloc(nzA,sizeof(int));
        sA_vals = (float *)calloc(nzA,sizeof(float));

        convert_to_coo(m,n, A, sA_rows, sA_cols, sA_vals);
#ifdef VERBOSE
        print_mat(m,n,A);
        print_mat_coo(nzA, sA_rows, sA_cols, sA_vals);
#endif


#endif
#endif 

 B = (float *)calloc(n,sizeof(float));
 if (B==NULL) {printf("Out of memory B! \n"); exit(1);}

   generate_mat(n,1,B,1);
#ifdef VERBOSE
   print_mat(n,1,B);
#endif 

 C = (float *)calloc(m,sizeof(float));
 if (C==NULL) {printf("Out of memory C! \n"); exit(1);}

//naive implementation 
#ifdef TIMING
  gettimeofday(&before, NULL); 
#endif

for (r=0; r<REP; r++) 
 /* Call the SpMV kernel. */
#ifdef CSR
  csr_spmv(m,sA_rows, sA_cols_idx, sA_vals, B, C); 
#elif CSC
  csc_spmv(n,sA_cols, sA_rows_idx, sA_vals, B, C);
#elif COO
  coo_spmv(nzA, sA_rows, sA_cols, sA_vals, B, C); 
#else 
  spmv(m,n,A,B,C);
#endif

#ifdef TIMING
  gettimeofday(&after, NULL);
  printf("Reference code: %10.6f seconds \n", ((after.tv_sec + (after.tv_usec / 1000000.0)) -
            (before.tv_sec + (before.tv_usec / 1000000.0)))/REP);

#endif

 if ((fc = fopen(argv[2], "wt")) == NULL) exit(3);    
// write_sparse(fc,n,m,C);
 write_vector(fc,m,C);
 fclose(fc);  

 free(A);
 free(B); 
 free(C);

}

