#ifndef PORTAILS_H
#define PORTAILS_H

#include "const.h"
#include "Utilitaires.h"
#include "jeu.h"

int calculDistance(int position1, int position2);
void choixTrajectoire(int lesX[], int lesY[], int xPomme, int yPomme, int *portail, int *distanceObjectifeX, int *distanceObjectifeY);

#endif // PORTAILS_H