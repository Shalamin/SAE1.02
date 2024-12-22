#include <stdio.h>
#include "generation.h"
#include "const.h"
#include "Utilitaires.h"

/************************************************/
/*				 FONCTIONS GENERATION           */
/*				 DU PLATEAU DE JEU              */
/************************************************/

void initPlateau(tPlateau plateau)
{
    // initialisation du plateau avec des espaces
    for (int i = 1; i <= LARGEUR_PLATEAU; i++)
    {
        for (int j = 1; j <= HAUTEUR_PLATEAU; j++)
        {
            plateau[i][j] = ((i == 1 && j != HAUTEUR_PLATEAU / 2) ||
                             (j == 1 && i != LARGEUR_PLATEAU / 2) ||
                             (i == LARGEUR_PLATEAU && j != HAUTEUR_PLATEAU / 2) ||
                             (j == HAUTEUR_PLATEAU && i != LARGEUR_PLATEAU / 2))
                                ? BORDURE
                                : VIDE;
        }
    }
    initPaves(plateau);
}

void dessinerPlateau(tPlateau plateau)
{
    // affiche eà l'écran le contenu du tableau 2D représentant le plateau
    for (int i = 1; i <= LARGEUR_PLATEAU; i++)
    {
        for (int j = 1; j <= HAUTEUR_PLATEAU; j++)
        {
            afficher(i, j, plateau[i][j]);
        }
    }
}
void initPaves(tPlateau plateau)
{
    for (int i = 0; i < NB_PAVES; i++)
    {

        // AJOUT DANS LE TABLEAU
        for (int dx = 0; dx < TAILLE_PAVES; dx++)
        {
            for (int dy = 0; dy < TAILLE_PAVES; dy++)
            {
                plateau[dx + lesPavesX[i]][dy + lesPavesY[i]] = BORDURE;
                pavesX[i * TAILLE_PAVES + dx] = lesPavesX[i] + dx; // on traite les pavés avec I ,puis la taille d'un pavé X les coordonnées X
                pavesY[i * TAILLE_PAVES + dy] = lesPavesY[i] + dy; // même chose mais avec Y
            }
        }
    }
}