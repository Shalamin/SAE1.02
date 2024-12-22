#ifndef DIRERCTION_H
#define DIRERCTION_H

#include "const.h"
#include "Utilitaires.h"
#include "jeu.h"
#include "mouvement.h"



void definirDirection(char *direction, char *derniereDirection, int *movX, int *movY, int ordreDeplacement, int lesX[], int lesY[]);
void demiTour(int lesX[], int lesY[], char directionDT, int *mov);

#endif // DIRERCTION_H