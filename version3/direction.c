#include <stdio.h>
#include <stdlib.h>
#include "direction.h"

void definirDirection(char *direction, char *derniereDirection, int *movX, int *movY, int ordreDeplacement, int lesX[], int lesY[])
{
    if (ordreDeplacement == 0 || ordreDeplacement == 2) // déplacement en X
    {
        if (*derniereDirection == DROITE)
        {
            if (*movX < 0)
            {
                demiTour(lesX, lesY, GAUCHE, &*movY);
                *movX += (TAILLE / 2) - 1;
                *direction = GAUCHE;
            }
            else
            {
                *direction = DROITE;
            }
        }
        else if (*derniereDirection == GAUCHE)
        {
            if (*movX > 0)
            {
                demiTour(lesX, lesY, DROITE, &*movY);
                *movX -= (TAILLE / 2) - 1;
                *direction = DROITE;
            }
            else
            {
                *direction = GAUCHE;
            }
        }
        else
        {
            *direction = (*movX > 0) ? DROITE : GAUCHE;
        }
    }
    else // déplacement en Y
    {
        if (*derniereDirection == BAS)
        {
            if (*movY < 0)
            {
                demiTour(lesX, lesY, HAUT, &*movX);
                *movY += (TAILLE / 2) - 1;
                *direction = HAUT;
            }
            else
            {
                *direction = BAS;
            }
        }
        else if (*derniereDirection == HAUT)
        {
            if (*movY > 0)
            {
                demiTour(lesX, lesY, BAS, &*movX);
                *movY -= (TAILLE / 2) - 1;
                *direction = BAS;
            }
            else
            {
                *direction = HAUT;
            }
        }
        else
        {
            *direction = (*movY > 0) ? BAS : HAUT;
        }
    }
    *derniereDirection = *direction;
}
void demiTour(int lesX[], int lesY[], char directionDT, int *mov)
{
    char directionDemiTour;
    switch (directionDT)
    {
    case HAUT:

        if ((lesX[0] + 1) != LARGEUR_PLATEAU)
        {
            directionDemiTour = DROITE;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = HAUT;
            for (int i = 0; i < (TAILLE / 2) - 1; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
            (*mov)--;
        }
        else
        {
            directionDemiTour = GAUCHE;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = HAUT;
            for (int i = 0; i < (TAILLE / 2) - 1; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
            (*mov)++;
        }
        break;
    case BAS:
        if ((lesX[0] + 1) != LARGEUR_PLATEAU)
        {
            directionDemiTour = DROITE;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = BAS;
            for (int i = 0; i < (TAILLE / 2) - 1; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
            (*mov)--;
        }
        else
        {
            directionDemiTour = GAUCHE;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = BAS;
            for (int i = 0; i < (TAILLE / 2) - 1; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
            (*mov)++;
        }
        break;
    case DROITE:
        if ((lesY[0] - 1) != 1)
        {
            directionDemiTour = HAUT;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = DROITE;
            for (int i = 0; i < (TAILLE / 2) - 1; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
            (*mov)++;
        }
        else
        {
            directionDemiTour = BAS;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = DROITE;
            for (int i = 0; i < (TAILLE / 2) - 1; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
            (*mov)--;
        }
        break;
    case GAUCHE:
        if ((lesY[0] - 1) != 1)
        {
            directionDemiTour = HAUT;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = GAUCHE;
            for (int i = 0; i < (TAILLE / 2) - 1; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
            (*mov)++;
        }
        else
        {
            directionDemiTour = BAS;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = GAUCHE;
            for (int i = 0; i < (TAILLE / 2) - 1; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
            (*mov)--;
        }
    }
}