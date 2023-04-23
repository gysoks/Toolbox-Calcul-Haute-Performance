#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

# define VRAI 1
# define FAUX 0
# define N 20

int min(int T[N]){
/* On utilise la fonction étudiée au TD1*/
    int Est_min[N];
    int min, k, j;
    #pragma omp parallel for
    for (int i=0; i<N; i++){
        Est_min[i] = VRAI;
    }
    
    #pragma omp parallel for private(j,k) /*k et j sont lues en parallèle lors du 'if (T[k]>T[j])' : il faut donc utiliser l'attribut private*/
    for (int i=0; i<N*N; i++){            /*On ne peut pas faire de for parallel sur 2 indices successifs, on prend donc k=i/n et j=i%n*/
        k=i/N;
        j=i%N;
        if (T[k]>T[j]){
            Est_min[k]= FAUX;
        }
    }
    #pragma omp parallel for
    for (int i=0; i<N; i++){
        if (Est_min[i]==VRAI){
            min=i;
        }
    }
    return min;
}

int main(void){
    int T[N];
    int j;
    int liste_triee[sizeof(T)];

    #pragma omp parallel for          /*Génération du tableau*/
    for (int i=0; i<N; i++){
        T[i]=rand()%101;
    }
    printf("Liste non-triée: \n");
    for (int i=0; i<20; i++){
        printf("%d, ", T[i]);
    }
    printf("\n");
    printf("\n");

    for (int i=0; i<sizeof(T); i++){  /*On insère successivement dans 'liste_triee' le min de T, qui change à chaque itération*/
        j=min(T);
        liste_triee[i]=T[j];
        T[j]=200;                     /*Il n'y a que des nombres entre 0 et 100, donc 200 sera un maximum inatteignable*/
    }
    printf("La liste triée est: \n");
    for (int i=0; i<20; i++){
        printf("%d, ", liste_triee[i]);
    }
    printf("\n");
}

/* L'algorithme de tri possède une compléxité parallele de O(1) sur chacune des boucles paralleles et 
elle recquiert N^2 processeurs*/