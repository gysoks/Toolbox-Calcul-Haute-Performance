#include <stdio.h>

int tab_zero_un(int T[]){
    int res = 1;
    #pragma omp parallel for
    for (int i=0; i<sizeof(T); i++){
        if (T[i] != 0 && T[i] != 1){
            res = 0;
        }
    }
    return res;
}


int main (void){
    int tab[10];
    for (int i=0; i<10; i++) {
        tab[i]=random();
        printf("%d\n", tab[i]);
    }
    int T2[10];
    for (int i=0; i<10; i++) {
        tab[i]=1; 
    }
    printf("%d\n",tab_zero_un(T2));
    return 0; 
}
