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
bool existePaves(int lesX[], int lesY[], int xPomme, int yPomme, char derniereDirection, tPlateau plateau);
void escivePaves(int lesX[], int lesY[], int xPomme, int yPomme, tPlateau plateau);
#endif // MOUVEMENT_H