//TP prog GPU séance 30/03

/*Fichier include*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>

FILE *FileDump;
int main(int argc, char** argv)
{
    int N=1024;
    float *A;
    float *B;
    float *C;
    float tmp;
    float s;
    int i,j,k;

    int nbthreads = 1;

    if (argc==1){
        nbthreads=atoi(argv[1]);
        omp_set_num_threads(nbthreads);
    }


    /*Allocation dynamique du tableau*/
    A=(float*)malloc(N*N*sizeof(float));
    B=(float*)malloc(N*N*sizeof(float));
    C=(float*)malloc(N*N*sizeof(float));

    /*Initialisation matrices*/
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            A[i+j*N]=1.0f;
            B[i+j*N]=1.0f;
        }
    }

    /*Calcul matrice C*/
    double tic=omp_get_wtime();

    #pragma omp parallel for private(i,j,k,tmp) shared(A,B,C,N)
    for (int i=0; i<N; i++)
    {
        for (int j=0; j<N; j++)
        {
            s=0.0f;
            for (int k=0; k<N; k++)
            {
                s+=B[i+k*N]*A[k+j*N];
            }
            C[i+j*N]= s;
        }
    }


    /*end timer*/
    double toc=omp_get_wtime();
    /*cpu time*/
    double ElapsedTime=(double)(toc-tic);
    double GFLOPS=2.0*pow((float)N,3)/ElapsedTime*1.0e-9;

    FileDump = fopen("mxm-omp.txt", "a+");
    fprintf(FileDump, "%d %e %f\n", N, ElapsedTime, GFLOPS);

    /*delocation dynamique*/
    free(A);
    free(B);
    free(C);
}
