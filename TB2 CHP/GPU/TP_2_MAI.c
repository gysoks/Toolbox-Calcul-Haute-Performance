/* TP CHP 2 Mai */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>

#define n 1024

void prod_mat_vec(float *A, float *x, float *C)
{
    float sum=0;
    /*On parallélise le for*/
    #pragma omp parallel for private(i,j) shared(C,n,sum)
    for (int i=0; i<n; i++){
        for (int j=0; j<n; j++){
            sum += A[i+j*n]*x[j];
        }
        C[i]=sum;
        sum=0;
    }
}

void saxpy(float a, float *x, float *y, int k, float *res)
{
    #pragma omp parallel for private(i,j) shared(k,C, res, sum)
    for (int i=0; i<n; i++){
         if (k==0){
            res[i]= y[i] + a*x[i];
        }
        else if (k==1){
            res[i]= y[i] - a*x[i];
        }
    }
}


float norme_1(float *a)
{
    float sum=0;

    #pragma omp parallel for private(i) shared(n, sum)
    for (int i=0; i<n; i++){
        sum += fabs(a[i]);
    }
    return sqrt(sum);
}

float prod_scal(float *a, float *b)
{
    float sum=0.0f;

    #pragma omp parallel for private(i) shared(n, sum) reduction(+:sum)
    for (int i=0; i<n; i++){
        sum += a[i]*b[i];
    }
    return sum;
}

FILE *FileDump;
int main(void){

    float *A;
    float *b;
    float *r;
    float *p;
    float *x;
    float *eps0;

    float *r_new;
    float *p_new;
    float *Ap;

    float alpha;
    float beta;

    float tmp;

    /*Allocation dynamique du tableau*/
    A=(float*)malloc(n*n*sizeof(float));
    b=(float*)malloc(n*sizeof(float));
    r=(float*)malloc(n*sizeof(float));
    p=(float*)malloc(n*sizeof(float));
    x=(float*)malloc(n*sizeof(float));
    r_new=(float*)malloc(n*sizeof(float));
    p_new=(float*)malloc(n*sizeof(float));
    Ap=(float*)malloc(n*sizeof(float));

    /*Initialisation des matrices*/
    for (int i=0; i<n; i++)
    {
        b[i]=1.0f;
        x[i]=1.0f;
        for (int j=0; j<n; j++)
        {
            A[i+j*n]=(float)(1.0)/(float)(i+j-1+1+1);
        }
    }
    prod_mat_vec(A, x, Ap);
    saxpy(1.0f, Ap, b, 1, r);
    
    for (int i=0; i<n; i++){
        p[i]=r[i];
    }

    /* Méthode Conjugate Gradient */
    double tic=omp_get_wtime();

    float p_scal_rj;
    for (int j=0; j<n; j++)
    {
        p_scal_rj=prod_scal(r, r);
        prod_mat_vec(A, p, Ap);
        alpha=p_scal_rj/(prod_scal(Ap, p));
        saxpy(alpha, p, x, 0, x);
        saxpy(alpha, Ap, r, 1, r_new);
        printf("%f", norme_1(r_new));
        if (norme_1(r) < *eps0){
            exit(1);
        }
        beta= prod_scal(r_new, r_new)/p_scal_rj;
        saxpy(beta, p, r_new, 0, p);
    }

    /*end timer*/
    double toc=omp_get_wtime();
    /*cpu time*/
    double ElapsedTime=(double)(toc-tic);
    double GFLOPS=2.0*pow((float)n,3)/ElapsedTime*1.0e-9;

    FileDump = fopen("mxm-omp.txt", "a+");
    fprintf(FileDump, "%d %e %f\n", n, ElapsedTime, GFLOPS);
    
    /*Délocation dynamique*/

    free(A);
    free(b);
    free(r);
    free(p);
    free(x);
    return 0;
}

double wtime(void)
{
    static int sec =-1;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    if (sec <0) sec = tv.tv_sec;
    return (tv.tv_sec) + 1.0e-6*tv.tv_usec;
}

