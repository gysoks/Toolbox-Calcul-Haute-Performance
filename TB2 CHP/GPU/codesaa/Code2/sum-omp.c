/*
-----------------------------------------
NAME OF THE PROGRAM : sum-omp.c
                      omp version
-----------------------------------------
*/
/*
-----------------------------------------
PURPOSE OF THE PROGRAM
Compute the sum of vector (1/n,.....,1/n)
-----------------------------------------
/*
-----------------------------------------
Author: A. Aoufi 
Date  : April 2016
-----------------------------------------
*/
/*
-----------------------------------------
LIST OF HEADERS FILES
-----------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>
/*
-----------------------------------------
LIST OF GLOBAL VARIABLES
-----------------------------------------
*/
#define SUCCESS 0
float *x;
unsigned int size;
int n;
FILE *FileDump;
/*
-----------------------------------------
PROTOTYPES OF FUNCTIONS
-----------------------------------------
*/
double wtime(void);
/*
-----------------------------------------
ENTRY POINT OF THE PROGRAM
-----------------------------------------
*/
int main(int argc,char **argv)
{
  double tic;
  double toc; 
  double ElapsedTime;
  double Sum=0.0f; 
  int    NbMaxThreads=1;
  int    nbthreads;
  unsigned int i;
  
  printf("argc= %d\n",argc);
  if ( argc == 1 )
    NbMaxThreads=4;
  else    
    NbMaxThreads=atoi(argv[1]);

  printf("NbMaxThreads= %d\n",NbMaxThreads);

  FileDump = fopen ("SumOmp.txt", "w");
  /* Loop over Number of vector elements */
  for(n=12;n<=24; n++) 
  {
    size=2<<(n-1);
    printf("\n");
    printf("n    = %ld\n",n);
    printf("2^n  = %ld\n",size);
    printf("size = %ld [Kb]\n",size*sizeof(float)/1024);
     
    x= (float*)calloc(size,sizeof(float));
    for (i=0 ; i < size ; i++)
      x[i]=1.0f;
  
    fprintf(FileDump,"%17d ",size);
    for(nbthreads= 1; nbthreads <= NbMaxThreads; nbthreads++)
    {
      /* allocate number of threads */
      omp_set_dynamic(0);
      omp_set_num_threads(nbthreads);
      /* start timer */
      tic=omp_get_wtime();
      /* parallelise reduction operation */
      Sum=0.0f;
      #pragma omp parallel for private(i) shared(x) reduction(+:Sum)
      for (i=0; i < size; i++)
         Sum+=x[i];
      /* end   timer */
      toc=omp_get_wtime();
      /* cpu time */
      ElapsedTime= (double)(toc - tic);
      printf("nbthreads=%d cputime=%le\n",nbthreads,ElapsedTime);
      printf("Sum      =%lf\n",Sum);
      printf("Exact= %le\n",(float)(size));
      fprintf(FileDump,"\t %le",ElapsedTime);
    }
    fprintf(FileDump,"\n");
    /* free dynamically allocated arrays */
    free(x);
  }
  fclose(FileDump);

  return SUCCESS;
}

double wtime(void)
{
   /* Use a generic timer */
   static int sec = -1;
   struct timeval tv;
   gettimeofday(&tv, NULL);
   if (sec < 0) sec = tv.tv_sec;
   return (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
}

