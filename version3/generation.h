#ifndef GENERATION_H
#define GENERATION_H

#include "const.h"
#include "types.h"
#include "globales.h"

void initPlateau(tPlateau plateau);
void dessinerPlateau(tPlateau plateau);
void ajouterPomme(tPlateau plateau, int *xPomme, int *yPomme, int nbPommes);
void initPaves(tPlateau plateau);

#endif // GENERATION_H