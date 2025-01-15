
#include <unistd.h>
#include "mouvement.h"
#include "direction.h"
#include "globales.h"
#include "const.h"

void progresser(int lesX[], int lesY[], char direction)
{
    // efface le dernier élément avant d'actualiser la position de tous les
    // élémentds du serpent avant de le  redessiner et détecte une
    // collision avec une pomme ou avec une bordure
    effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);

    // fait anvencer chaque élément de la queue
    for (int i = TAILLE - 1; i > 0; i--)
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }

    // faire progresser la tete dans la nouvelle direction en prenant en conte les portail
    switch (direction)
    {
    case HAUT:
        lesY[0] = (lesY[0] - 1);
        break;
    case BAS:
        lesY[0] = (lesY[0] + 1);
        break;
    case DROITE:
        lesX[0] = (lesX[0] + 1);
        break;
    case GAUCHE:
        lesX[0] = (lesX[0] - 1);
        break;
    }
    // dessine le serpent
    dessinerSerpent(lesX, lesY);
    fflush(stdout);
    if (((lesX[0] == 0 || lesX[0] == LARGEUR_PLATEAU + 1) && lesY[0] == HAUTEUR_PLATEAU / 2))
    {
        lesX[0] = valAbsolu(lesX[0] - (LARGEUR_PLATEAU - 1));
    }
    else if ((lesY[0] == 0 || lesY[0] == HAUTEUR_PLATEAU + 1) && lesX[0] == LARGEUR_PLATEAU / 2)
    {
        lesY[0] = valAbsolu(lesY[0] - (HAUTEUR_PLATEAU - 1));
    }
    usleep(ATTENTE);
}

void gestionCollisions(int lesX[], int lesY[], tPlateau plateau, bool *collision, bool *pomme)
{
    // détection d'une "collision" avec une pomme
    if (plateau[lesX[0]][lesY[0]] == POMME)
    {
        *pomme = true;
        // la pomme disparait du plateau
        plateau[lesX[0]][lesY[0]] = VIDE;
    }
    // détection d'une collision avec la bordure
    else if (plateau[lesX[0]][lesY[0]] == BORDURE)
    {
        *collision = true;
    }
    else
    {
        for (int i = 1; i < TAILLE; i++)
        {
            if (lesX[0] == lesX[i] && lesY[0] == lesY[i])
            {
                *collision = true;
            }
        }
    }
}