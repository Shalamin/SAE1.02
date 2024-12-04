#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TAILLE 300000
void tri_insertion(int tableau[]);
void génération(int tableau[]);
int main(){
  time_t debut_t = time(NULL);
  int tableau[TAILLE] ;
  srand(time(NULL));

  tri_insertion(tableau);
  for(int i = 0; i < TAILLE; i++){
    printf("%d\n", tableau[i]);
  }
  time_t fin_t = time(NULL);
  double total = ((fin_t - debut_t)*1.0);
  printf("Le temps total à été de %.3f secondes", total);
}
void génération(int tableau[]){
  for(int i = 0; i < TAILLE; i++){
    tableau[i] = rand()%TAILLE;
  }
}
void tri_insertion(int tableau[]){

  int x,j;
  for (int i = 1; i <= TAILLE-1; i++){
   // x représente l'élémentà placer au bon endroit
  x = tableau[i];
  //décaer les éléemnts T[O]...T[i-1] qui sont plus grands
  //que x , en partant de T[i-1]
  j = i;
  while(j>0 && tableau[j-1] > x){
    tableau[j] = tableau[j-1];
    j = j-1;
  }
  tableau[j] = x;
}
}
