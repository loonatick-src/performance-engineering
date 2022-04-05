#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mmio.h"

#define N  512
#define M  512

#define REP 10


void transpose(int m, int n, float *A, float *B) {
   int i, j;

   for(i=0; i<m; i++) {
      for(j=0; j<n; j++) {
         B[i+j*m] = A[i*n+j];
      }
   }
}

void generate_mat(int m, int n, float *A) {
  int i;

  for (i=0; i<(m*n); i++) A[i] = 1; //i/10; 

}

void read_sparse(FILE *f, int m, int n, int nz, float *A) {
  int i, row, col;
  float val;  
 
    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    for (i=0; i<nz; i++)
    {
        fscanf(f, "%d %d %f\n", &row, &col, &val);
        A[(row-1)*n+col-1] = val;   /* adjust from 1-based to 0-based */
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

void read_dense(FILE *f, int m, int n, float *A) {
  int row, col;

  for(row=0; row<m; row++) { 
     for (col=0; col<n; col++) {
        fscanf(f, "%f ", &A[row*n+col]); 
//	printf("%20.19f \n", A[row*(*n)+col]);
     }
  } 
}


int read_mat(int *m, int *n, int *nzA, FILE* fa) {
  MM_typecode ta;
  int ret_code; 

  if (mm_read_banner(fa, &ta) != 0)
    {
        printf("Could not process Matrix Market banneri for A.\n");
        return -3;
    }
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
    /* find out size of sparse matrix .... */
}

int main (int argc, char** argv) {
 float *A, *C;
 struct timeval before, after;
 int m, n, err;
 int nzA=0;
 FILE *fa, *fc; 
 
#ifdef GENERATE 
 m=M; n=N;
#else 
 if (argc < 3) {
    fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
    exit(1);
 }
 else {
    if ((fa = fopen(argv[1], "rt")) == NULL) exit(1);
    err = read_mat(&m, &n, &nzA, fa);    
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
   generate_mat(m,n,A);
#else 
   if (nzA>0)
	read_sparse(fa, m,n,nzA, A);
   else 
	read_dense(fa, m,n, A);
   fclose(fa); 
#endif

 C = (float *)calloc(m*n,sizeof(float));
 if (C==NULL) {printf("Out of memory C1! \n"); exit(1);}
// C2 = (float *)calloc(N*P,sizeof(float));
// if (C2==NULL) {printf("Out of memory C2! \n"); exit(1);}

//naive implementation 
#ifdef TIMING
  gettimeofday(&before, NULL); 
#endif

//for (r=0; r<REP; r++) 
 transpose(m,n,A,C);

#ifdef TIMING
  gettimeofday(&after, NULL);
  printf("Reference code: %10.2f seconds \n", ((after.tv_sec + (after.tv_usec / 1000000.0)) -
            (before.tv_sec + (before.tv_usec / 1000000.0)))/REP);

#endif

 if ((fc = fopen(argv[2], "wt")) == NULL) exit(3);    
 write_sparse(fc,n,m,C);
 fclose(fc);  

 free(A);
 free(C);
// free(C2);

}

