//
// Created by KERAUDREN johan on 20/11/2024.
//
#include <stdio.h>

#define TAILLE 10;

int main() {

}
void tamiser(int tableau[],int noeud,int  n){
    int tempon;
    int fils = 2*noeud + 1;
    if(fils < n && tableau[fils] < tableau[fils + 1]){
      fils = fils + 1;
    }
    if(fils <= n && tableau[noeud] < tableau[fils]){
      tempon = tableau[fils];
      tableau[fils] = tableau[noeud];
      tableau[noeud] = tempon;
      tamiser(tableau,fils,n);
    }
}
void triParTas(int tableau[]){
  for(int i= (TAILLE/2)- 1;i > 0 ;i--){
    tamiser(tableau,i,TAILLE/2);
}
