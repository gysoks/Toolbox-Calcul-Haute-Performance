#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

# define VRAI 1                         /*Introduction de variables booléennes*/
# define FAUX 0
# define NB_APPRENTIS_CUISINE 6         /*Nombre de personnes travaillant en parallèle sur l'écossage*/

int main(int argc, char *argv[])
{
	int n;
	
	int a_faire, actif, tid; 

    /* int volume_travail[NB_APPRENTIS_CUISINE]; */  /*On crée une liste qui va stocker le volume de travail de chaque thread*/        

	if (argc < 2) /*Interaction avec l'utilisateur compilant le programme*/
	{
		printf(" on attend un nombre d'unités de petits pois\n");
		exit(2);
	}
	
	n = atoi(argv[1]);
	a_faire = n;


			
#pragma omp parallel default(shared) private(tid, actif) num_threads(NB_APPRENTIS_CUISINE)
/* Cette partie du code représente le travail en parallèle d'écossage des petits pois: 
   'tid' représente le numéro dans la file des travailleurs, 'actif' la variable booléenne
   qui définit si le travail est fini ou non et 'NB_APPRENTIS_CUISINE' définis le nombre de 
   travailleurs. Le travail est fini lorsque tous les travailleurs ont fini leur tâche (actif = FAUX && a_faire =0)*/
{
	tid = omp_get_thread_num();
	while (a_faire >0)
	{
		actif = FAUX;
		# pragma omp critical
		{
			if (a_faire >0) /*Il reste des petits pois à écosser*/
			{
				a_faire --;
				actif = VRAI;
                /* volume_travail[tid] ++; */ /*Dès qu'un petit pois est écossé par la thread n°tid, on incrémente le compteur de volume de travail associé de +1*/
			}
				
		}
		if (actif == VRAI)   /*Le travail n'est pas fini*/
			printf(" je suis no=%d et j'ai une unité à écosser.\n", tid);
		else
			printf(" no = %d, je n'ai plus de travail", tid);
	}	
}
    /* for (int i=0, i<NB_APPRENTIS_CUISINE; i++){
        printf( "Je suis no=%d et j'ai écossé %d petits pois \n", i, volume_travail[i]); */
    }
    printf(" Nous avons terminé, a_faire = %d\n", a_faire);
}