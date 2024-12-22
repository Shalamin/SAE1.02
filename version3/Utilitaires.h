#ifndef UTILITAIRES_H
#define UTILITAIRES_H

#include "const.h"
#include "globales.h"

void afficher(int, int, char);
void effacer(int x, int y);
int valAbsolu(int valeur);
void gotoxy(int x, int y);
int kbhit();
void disableEcho();
void enableEcho();

#endif // UTILITAIRES_H