/*
-----------------------------------------
PURPOSE OF THE PROGRAM
Compute the sum of vector (1/n,.....,1/n)
-----------------------------------------
*/
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
/*
-----------------------------------------
LIST OF GLOBAL VARIABLES
-----------------------------------------
*/
#define SUCCESS 0
float *x;
unsigned long int size;
unsigned long int i;
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
int main(void)
{
  double tic;
  double toc; 
  double ElapsedTime;
  double Sum=0.0f; 

  FileDump = fopen ("SumScalar.txt", "w");
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
  
    Sum=0.0f;
    /* start timer */ 
    tic = wtime();
    for (i=0 ; i < size ; i++)
      Sum+=x[i];
    /* end   timer */
    toc=wtime();
    /* cpu time */ 
    ElapsedTime= (double)(toc - tic);
    printf("Time = %le [s]\n",ElapsedTime);
    printf("Sum  = %le\n",Sum);
    printf("Exact= %le\n",(float)(size));
    fprintf(FileDump,"%ld %e\n",size,ElapsedTime);
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

