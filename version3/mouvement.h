#ifndef MOUVEMENT_H
#define MOUVEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "const.h"
#include "Utilitaires.h"
#include "jeu.h"

void progresser(int lesX[], int lesY[], char direction);
void gestionCollisions(int lesX[], int lesY[], tPlateau plateau, bool *collision, bool *pomme);
void demiTour(int lesX[], int lesY[], char direction, int *mov);


#endif // MOUVEMENT_H