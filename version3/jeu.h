#ifndef JEU_H
#define JEU_H
#include <stdio.h>
#include <time.h>
#include "const.h"
#include "types.h"
#include "generation.h"



void dessinerSerpent(int lesX[], int lesY[]);
void ajouterPomme(tPlateau plateau, int *xPomme, int *yPomme, int nbPommes);
void finDuJeu(int numeroPomme, time_t debut_t, time_t fin_t, int nbDeplacement);

#endif // JEU_H