//
// Created by KERAUDREN johan on 20/11/2024.
//
#include <stdio.h>
#include <time.h>

#define TAILLE 12

int partition(int tableau[], int debut, int fin, int pivot);
void triRapide(int tableau[TAILLE],int debut,int fin);


int main(){
  time_t debut_t = time(NULL);

  int tableau[TAILLE]={5,3,9,8,7,5,2,1,9,1};
  int debut = 0;
  int fin = TAILLE;
  triRapide(tableau,debut,fin);
  for(int i = 0; i < TAILLE; i++){
    printf("%d\n", tableau[i]);
  }
  time_t fin_t = time(NULL);
  double total = ((fin_t - debut_t)*1.0);
  printf("Le temps total à été de %.3f secondes", total);
}
int partition(int tableau[], int debut, int fin, int pivot){
  int tempon;
  tempon = tableau[pivot];
  tableau[pivot] = tableau[fin];
  tableau[fin] = tempon;
  int j = debut;
  for(int i = debut; i < fin; i++){
    if(tableau[i] <= tableau[fin]){
      tempon = tableau[i];
      tableau[i] = tableau[j];
      tableau[j] = tempon;
      j= j+1;
    }
  }
      tempon = tableau[fin];
      tableau[fin] = tableau[j];
      tableau[j] = tempon;
      return j;
}

 void triRapide(int tableau[TAILLE],int debut,int fin){
   int pivot;
   if(debut < fin){
     pivot = (debut+fin)/2;
     pivot = partition(tableau,debut,fin,pivot);
     triRapide(tableau,debut,pivot-1);
     triRapide(tableau,pivot+1,fin);
   }



}