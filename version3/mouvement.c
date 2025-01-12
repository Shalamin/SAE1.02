
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
        lesX[0] = valAbsolu(lesX[0] - (LARGEUR_PLATEAU + 1));
    }
    else if ((lesY[0] == 0 || lesY[0] == HAUTEUR_PLATEAU + 1) && lesX[0] == LARGEUR_PLATEAU / 2)
    {
        lesY[0] = valAbsolu(lesY[0] - (HAUTEUR_PLATEAU + 1));
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
bool existePaves(int lesX[], int lesY[], int xPomme, int yPomme, char derniereDirection, tPlateau plateau)
{
    int distPommeY = lesY[0] - yPomme;
    distPommeY = valAbsolu(distPommeY);
    int distPommeX = lesX[0] - xPomme;
    distPommeX = valAbsolu(distPommeX);
    // vérifie si le serpent peut atteindre la pomme sans passer par un obstacle
    bool existePaves = false;
    if (derniereDirection == GAUCHE || derniereDirection == DROITE)
    {

        for (int i = 0; i < distPommeY; i++)
        {
            // vérifie si le serpent peut atteindre la pomme sans passer par un obstacle en Y
            if (plateau[lesX[0]][lesY[0] + i] == BORDURE)
            {
                existePaves = true;
            }

            else if (plateau[lesX[0]][lesY[0] - i] == BORDURE)
            {
                existePaves = true;
            }
        }
    }
    else
    {

        for (int i = 0; i < distPommeX; i++)
        {
            if (plateau[lesX[0] + i][lesY[0]] == BORDURE)
            {
                existePaves = true;
            }

            else if (plateau[lesX[0] - i][lesY[0]] == BORDURE)
            {
                existePaves = true;
            }
        }
    }
    return existePaves;
}

void escivePaves(int lesX[], int lesY[], int xPomme, int yPomme, tPlateau plateau)
{
    int distPommeX = xPomme - lesX[0];
    int distPommeY = yPomme - lesY[0];
    int movX = valAbsolu(distPommeX);
    int movY = valAbsolu(distPommeY);
    int movGauche = 0, movDroite = 0;
    int movHaut = 0, movBas = 0;
    // Vérifie les mouvements nécessaires pour esquiver à gauche et à droite
    if (movX == 0 && movY != 0)
    {

        for (int i = 1; i <= movY; i++)
        {
            if (plateau[lesX[0] - movGauche][lesY[0] - i] == BORDURE || plateau[lesX[0] - movGauche][lesY[0] + i] == BORDURE)
            {
                movGauche++;
            }
        }
        for (int i = 1; i <= movY; i++)
        {
            if (plateau[lesX[0] + movDroite][lesY[0] - i] == BORDURE || plateau[lesX[0] + movDroite][lesY[0] + i] == BORDURE)
            {
                movDroite++;
            }
        }
    }
    if (movY == 0 && movX != 0)
    {
        for (int i = 1; i <= movX; i++)
        {
            if (plateau[lesX[0] - i][lesY[0] - movHaut] == BORDURE || plateau[lesX[0] + i][lesY[0] - movHaut] == BORDURE)
            {
                movHaut++;
            }
        }
        for (int i = 1; i <= movX; i++)
        {
            if (plateau[lesX[0] - i][lesY[0] + movBas] == BORDURE || plateau[lesX[0] + i][lesY[0] + movBas] == BORDURE)
            {
                movBas++;
            }
        }
    }
    // Choisit la direction avec le moins de mouvements nécessaires
    if (movGauche < movDroite)
    {
        for (int i = 0; i < movGauche; i++)
        {
            progresser(lesX, lesY, GAUCHE);
        }
    }
    else
    {
        for (int i = 0; i < movDroite; i++)
        {
            progresser(lesX, lesY, DROITE);
        }
    }
    if (movBas < movHaut)
    {
        for (int i = 0; i < movBas; i++)
        {
            progresser(lesX, lesY, BAS);
        }
    }
    else
    {
        for (int i = 0; i < movHaut; i++)
        {
            progresser(lesX, lesY, HAUT);
        }
    }
}
