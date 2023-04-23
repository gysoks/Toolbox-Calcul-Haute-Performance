#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

# define VRAI 1
# define FAUX 0
# define NB_APPRENTIS_CUISINE 6

int main(int argc, char *argv[])
{
	int n;
	
	int a_ramasser, actif, tid, a_laver, a_denoyoter; /* On établit une variable pour les abricots à laver, une pour ceux à ramasser et une pour ceux à dénoyoter*/

	if (argc < 2)
	{
		printf(" on attend un nombre d'unités d'abricots\n");
		exit(2);
	}
	
	n = atoi(argv[1]);
	a_denoyoter = 0;
    a_laver = 0;
    a_ramasser =n;

			
#pragma omp parallel default(shared) private(tid, actif) num_threads(NB_APPRENTIS_CUISINE)
{
	tid = omp_get_thread_num();
    
    while (a_ramasser > 0)
	{
        #pragma omp parallel
        {
            a_ramasser--;
            a_laver++;
        }
    }
    #pragma omp barrier

    #pragma omp master 
{
    while (a_laver >0 )
	{
        {
            a_laver--;
            a_denoyoter++;
        }
    }
}

    #pragma omp barrier
    while (a_denoyoter > 0)  {
        # pragma omp critical
		{
			if (a_denoyoter >0) 
			{
				a_denoyoter --;
			}
        }	
		}
}
    printf(" Nous avons terminé: a_denoyoter = %d, a_laver = %d, a_ramasser = %d\n", a_denoyoter, a_laver, a_ramasser);
}





