#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "mmio.h"

// macro to acces the index at y, x for matrix A
#define IA(y, x) A[y*n+x]

// macro to acces the index at y, x for matrix B
#define IB(y, x) B[y*p+x]

// macro to acces the index at y, x for matrix C
#define IC(y, x) C[y*p+x]

// replace these with the hardware version for cuda/opencl
#define min(x, y) (x < y ? x : y)
#define max(x, y) (x > y ? x : y)

#define GENERATE
#define TIMING
#define N  512
#define M  512
#define P  512

#define REP 1

#define BYTES_PER_CACHE_LINE 64

void matrix_mult(int m, int n, int p, float *A, float *B, float *C) {
    // cache line size used to find the optimal block size
    const int block_size = BYTES_PER_CACHE_LINE / sizeof(float);
    
    // step 1, clear the matrix
    // IEEE defines a float with value 0.0 as 32 bits set to 0
    memset(C, 0, sizeof(float) * m * p);
    
    // create 3 blocks
    // 1 for the resulting block of C,
    // and 2 for the temperary blocks of A and B
    // ...
    
    // the resulting array is a M * P matrix
    // these 2 outer loops can be run in parallel with openmp
    // #pragma omp parallel for collapse(3)
    for (int y = 0; y < m; y += block_size) {
        for (int x = 0; x < p; x += block_size) {
            for (int k = 0; k < n; k += block_size) {
                // find block dimension
                int block_y_start = y,
                    block_y_end = min(m, y + block_size);
                int block_x_start = x,
                    block_x_end = min(p, x + block_size);
                int block_k_start = k,
                    block_k_end = min(n, k + block_size);
                
                // loop over the target block and add the dot
                // products of the rows and columns in the blocks
                for (int by = block_y_start; by < block_y_end; by++) {
                    for (int bx = block_x_start; bx < block_x_end; bx++) {
                        float dot = 0;
                        for (int bk = block_k_start; bk < block_k_end; bk++) {
                            // by is the same row as in matrix A
                            // bx is the same col as in matrix C
                            // bk is the index in the row/col
                            dot += IA(by, bk) * IB(bk, bx);
                        }
                        // this magic line allows us to also add the blocks in parallel
                        // #pragma omp atomic
                        IC(by, bx) += dot;
                    }
                }
                
                // sync threads before moving on to the next block
            }
        }
    }
}




void generate_mat(int m, int n, int p, float *A, float *B) {
    int i;
    
    for (i=0; i<(m*n); i++) A[i] = 1; //i/10;
    for (i=0; i<(n*p); i++) B[i] = 1; //i/5;
    
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
            //    printf("%20.19f \n", A[row*(*n)+col]);
        }
    }
}


int read_mat(int *m, int *n, int *p, int *nzA, int *nzB, FILE* fa, FILE *fb) {
    MM_typecode ta, tb;
    int ret_code;
    int n1;
    
    if (mm_read_banner(fa, &ta) != 0)
    {
        printf("Could not process Matrix Market banneri for A.\n");
        return -3;
    }
    if (mm_read_banner(fb, &tb) != 0)
    {
        printf("Could not process Matrix Market banner for B.\n");
        return -4;
    }
    
    if (mm_is_complex(ta)) return -6;
    if (mm_is_complex(tb)) return -7;
    
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
    
    
    if (mm_is_matrix(tb) && mm_is_sparse(tb))
    {
        if ((ret_code = mm_read_mtx_crd_size(fb, &n1, p, nzB)) !=0)
            return -10;
    }
    else if (mm_is_matrix(tb) && mm_is_array(tb)) {
        *nzB = 0;
        if ((ret_code = mm_read_mtx_array_size(fb, &n1, p)) !=0)
            return -11;
        
    }
    else return -9;
    
    if (*n!=n1) return -15;
    
    return 0;
    /* find out size of sparse matrix .... */
}

int main (int argc, char** argv) {
    float *A, *B, *C;
#ifdef TIMING
    struct timeval before, after;
#endif
    int m, n, p, r, err;
    int nzA=0, nzB=0;
    FILE *fa, *fb, *fc;
    
#ifdef GENERATE
    m=M; n=N; p=P;
#else
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [martix1] [matrix2] [resultmatrix] \n", argv[0]);
        exit(1);
    }
    else {
        if ((fa = fopen(argv[1], "rt")) == NULL) exit(1);
        if ((fb = fopen(argv[2], "rt")) == NULL) exit(2);
        err = read_mat(&m, &n, &p, &nzA, &nzB, fa,fb);
        if (err == -15) {
            printf("Matrices are incompatible! \n");
            fclose(fa); fclose(fb);
            exit(1);
        }
    }
#endif
    
    A = (float *)calloc(m*n,sizeof(float));
    if (A==NULL) {printf("Out of memory A! \n"); exit(1);}
    B = (float *)calloc(n*p,sizeof(float));
    if (B==NULL) {printf("Out of memory B! \n"); exit(1);}
    
#ifdef GENERATE
    generate_mat(m,n,p,A,B);
#else
    if (nzA>0)
        read_sparse(fa, m,n,nzA, A);
    else
        read_dense(fa, m,n, A);
    if (nzB>0)
        read_sparse(fb, n,p, nzB, B);
    else
        read_dense(fb, n,p, B);
    fclose(fa);
    fclose(fb);
#endif
    
    C = (float *)calloc(m*p,sizeof(float));
    if (C==NULL) {printf("Out of memory C1! \n"); exit(1);}
    // C2 = (float *)calloc(N*P,sizeof(float));
    // if (C2==NULL) {printf("Out of memory C2! \n"); exit(1);}
    
    //naive implementation
#ifdef TIMING
    printf("Starting\n");
    gettimeofday(&before, NULL);
#endif
    
    for (r=0; r<REP; r++)
        matrix_mult(m,n,p,A,B,C);
    
#ifdef TIMING
    gettimeofday(&after, NULL);
    printf("Reference code: %10.2f seconds \n", ((after.tv_sec + (after.tv_usec / 1000000.0)) -
                                                 (before.tv_sec + (before.tv_usec / 1000000.0)))/REP);
    
#endif
    
#ifdef GENERATE
    if ((fc = fopen("gen_result.mtx", "wt")) == NULL) exit(3);
#else
    if ((fc = fopen(argv[3], "wt")) == NULL) exit(3);
#endif
    write_sparse(fc,m,p,C);
    fclose(fc);
    
    free(A);
    free(B);
    free(C);
    // free(C2);
    
}