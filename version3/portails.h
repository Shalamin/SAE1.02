#ifndef PORTAILS_H
#define PORTAILS_H

#include "const.h"
#include "Utilitaires.h"
#include "jeu.h"

int calculDistance(int position1, int position2);
void choixPortail(int lesX[], int lesY[], int xPomme, int yPomme, int *portail, int *distancePommeX, int *distancePommeY, int *distancePortailX, int *distancePortailY);


#endif // PORTAILS_H