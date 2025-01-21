
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
bool existePavesPomme(int lesX[], int lesY[], int xPomme, int yPomme, char derniereDirection, tPlateau plateau)
{
    int distPommeY = lesY[0] - yPomme;
    distPommeY = valAbsolu(distPommeY);
    int distPommeX = lesX[0] - xPomme;
    distPommeX = valAbsolu(distPommeX);
    // vérifie si le serpent peut atteindre la pomme sans passer par un obstacle
    bool existePaves = false;
    if (derniereDirection == GAUCHE || derniereDirection == DROITE)
    {
        // pour annuler la priorité en Y
        for (int i = 0; i < distPommeY; i++)
        {
            // vérifie si le serpent peut atteindre la pomme sans passer par un obstacle en Y
            if (plateau[lesX[0]][lesY[0] + i] == PAVE)
            {
                existePaves = true;
            }

            else if (plateau[lesX[0]][lesY[0] - i] == PAVE)
            {
                existePaves = true;
            }
        }
    }
    else
    {

        for (int i = 0; i < distPommeX; i++)
        {
            if (plateau[lesX[0] + i][lesY[0]] == PAVE)
            {
                existePaves = true;
            }

            else if (plateau[lesX[0] - i][lesY[0]] == PAVE)
            {
                existePaves = true;
            }
        }
    }
    if ((derniereDirection == BAS || derniereDirection == HAUT))
    {
        // Si on allait vers le bas ou le haut et qu'on a trouvé un pavé en X,
        // on vérifie aussi le chemin en Y
        for (int i = 0; i < distPommeY; i++)
        {
            if (plateau[lesX[0]][lesY[0] + i] == PAVE)
            {
                existePaves = true;
            }
            else if (plateau[lesX[0]][lesY[0] - i] == PAVE)
            {
                existePaves = true;
            }
            else
            {
                existePaves = false;
                gotoxy(1, HAUTEUR_PLATEAU + 2);
                //printf("j'existe\n");
            }
        }
    }

    return existePaves;
}
bool existePavesPortail(int lesX[], int lesY[], int xPortail, int yPortail, char derniereDirection, tPlateau plateau)
{
    int distPortailY = lesY[0] - yPortail;
    distPortailY = valAbsolu(distPortailY);
    int distPortailX = lesX[0] - xPortail;
    distPortailX = valAbsolu(distPortailX);
    // vérifie si le serpent peut atteindre la pomme sans passer par un obstacle
    bool existePaves = false;
    if (derniereDirection == GAUCHE || derniereDirection == DROITE)
    {
        // pour annuler la priorité en Y
        for (int i = 0; i < distPortailY; i++)
        {
            // vérifie si le serpent peut atteindre la pomme sans passer par un obstacle en Y
            if (plateau[lesX[0]][lesY[0] + i] == PAVE)
            {
                existePaves = true;
            }

            else if (plateau[lesX[0]][lesY[0] - i] == PAVE)
            {
                existePaves = true;
            }
        }
    }
    else if (derniereDirection == HAUT || derniereDirection == BAS)
    {

        for (int i = 0; i < distPortailX; i++)
        {
            if (plateau[lesX[0] + i][lesY[0]] == PAVE)
            {
                existePaves = true;
            }

            else if (plateau[lesX[0] - i][lesY[0]] == PAVE)
            {
                existePaves = true;
            }
        }
    } // pour annuler la priorité en X
    if ((derniereDirection == BAS || derniereDirection == HAUT) && existePaves)
    {
        // Si on allait vers le bas ou le haut et qu'on a trouvé un pavé en X,
        // on vérifie aussi le chemin en Y
        for (int i = 0; i < distPortailY; i++)
        {
            if (plateau[lesX[0]][lesY[0] + i] == PAVE)
            {
                existePaves = true;
            }
            else if (plateau[lesX[0]][lesY[0] - i] == PAVE)
            {
                existePaves = true;
            }
            else
            {
                existePaves = false;
            }
        }
    }

    return existePaves;
}
void escivePavesPomme(int lesX[], int lesY[], int xPomme, int yPomme, char *derniereDirection, tPlateau plateau, int *axeEscive)
{
    int distPommeX = xPomme - lesX[0];
    int distPommeY = yPomme - lesY[0];
    int movX = valAbsolu(distPommeX);
    int movY = valAbsolu(distPommeY);
    int movGauche = 0, movDroite = 0;
    int movHaut = 0, movBas = 0;

    // Vérifie les mouvements nécessaires pour esquiver à gauche
    if (movX == 0 && movY != 0)
    {
        //verifie si le paver a éviter est en haut ou en bas
        if(distPommeY > 0)
        {
            for (int i = 0; i <= movY; i++)
            {
                if (plateau[lesX[0] - movGauche][lesY[0] + i] == PAVE)
                {
                    movGauche++;
                    i--;
                }
            }
            for (int i = 0; i <= movY; i++)
            {
                if (plateau[lesX[0] + movDroite][lesY[0] + i] == PAVE)
                {
                    movDroite++;
                    i--;
                }
            }
        }
        else
        {
            for (int i = 0; i <= movX; i++)
            {
                if (plateau[lesX[0] - movGauche][lesY[0] - i] == PAVE)
                {
                    movGauche++;
                    i--;
                }
            }
            for (int i = 0; i <= movX; i++)
            {
                if (plateau[lesX[0] + movDroite][lesY[0] - i] == PAVE )
                {
                    movDroite++;
                    i--;
                }
            }
        }
        
    }
    if (movY == 0 && movX != 0)
    {
        if(distPommeX > 0)
        {
            for (int i = 0; i <= LARGEUR_PLATEAU; i++)
            {
                if (plateau[lesX[0] + i][lesY[0] - movHaut] == PAVE)
                {
                    movHaut++;
                    i--;
                }
            }
            for (int i = 0; i <= LARGEUR_PLATEAU; i++)
            {
                if (plateau[lesX[0] + i][lesY[0] + movBas] == PAVE)
                {
                    movBas++;
                    i--;
                }
            }
        }
        else
        {
            for (int i = 0; i <= LARGEUR_PLATEAU; i++)
            {
                if (plateau[lesX[0] - i][lesY[0] - movHaut] == PAVE)
                {
                    movHaut++;
                    i--;
                }
            }
            for (int i = 0; i <= LARGEUR_PLATEAU; i++)
            {
                if (plateau[lesX[0] - i][lesY[0] + movBas] == PAVE)
                {
                    movBas++;
                    i--;
                }
            }
        }
        
    }
    if (movX != 0 && movY != 0)
    {
        if (*derniereDirection != DROITE)
        {
            for (int i = 0; i <= 5; i++)
            {
                if (plateau[lesX[0] - movGauche][lesY[0] - i] == PAVE || plateau[lesX[0] - movGauche][lesY[0] + i] == PAVE)
                {
                    movGauche++;
                }
            }
        }

        if (*derniereDirection != GAUCHE)
        {
            for (int i = 0; i <= 5; i++)
            {
                if (plateau[lesX[0] + movDroite][lesY[0] - i] == PAVE || plateau[lesX[0] + movDroite][lesY[0] + i] == PAVE)
                {
                    movDroite++;
                }
            }
        }
        if (*derniereDirection != BAS)
        {
            for (int i = 0; i <= 5; i++)
            {
                if (plateau[lesX[0] - i][lesY[0] - movHaut] == PAVE || plateau[lesX[0] + i][lesY[0] - movHaut] == PAVE)
                {
                    movHaut++;
                }
            }
        }
        if (*derniereDirection != DROITE)
        {
            for (int i = 0; i <= 5; i++)
            {
                if (plateau[lesX[0] - i][lesY[0] + movBas] == PAVE || plateau[lesX[0] + i][lesY[0] + movBas] == PAVE)
                {
                    movBas++;
                }
            }
        }
    }

    // Choisit la direction avec le moins de mouvements nécessaires
    if (movGauche < movDroite || movGauche == 0)
    {
        for (int i = 0; i < movGauche; i++)
        {
            progresser(lesX, lesY, GAUCHE);
            *derniereDirection = GAUCHE;
        }
        *axeEscive = 0;
    }
    else
    {
        for (int i = 0; i < movDroite; i++)
        {
            progresser(lesX, lesY, DROITE);
            *derniereDirection = DROITE;
        }
        *axeEscive = 0;
    }
    if (movBas < movHaut || movBas == 0)
    {
        for (int i = 0; i < movBas; i++)
        {
            progresser(lesX, lesY, BAS);
            *derniereDirection = BAS;
        }
        *axeEscive = 1;
    }
    else
    {
        for (int i = 0; i < movHaut; i++)
        {
            progresser(lesX, lesY, HAUT);
            *derniereDirection = HAUT;
        }
        *axeEscive = 1;
    }
}
void escivePavesPortail(int lesX[], int lesY[], int xPortail, int yPortail, char *derniereDirection, tPlateau plateau, int *axeEscive)
{
    int distPortailX = lesX[0] - xPortail;
    int distPortailY = lesY[0] - yPortail;
    int movX = valAbsolu(distPortailX);
    int movY = valAbsolu(distPortailY);
    int movGauche = 0, movDroite = 0;
    int movHaut = 0, movBas = 0;

    // Calcul des mouvements pour esquiver les pavés en direction du portail
    if (movX == 0 && movY != 0) // Déplacement vertical
    {
        for (int i = 0; i <= movY; i++)
        {
            if (plateau[lesX[0] - movGauche][lesY[0] - i] == PAVE || plateau[lesX[0] - movGauche][lesY[0] + i] == PAVE)
            {
                movGauche++;
            }
        }
        for (int i = 0; i <= movY; i++)
        {
            if (plateau[lesX[0] + movDroite][lesY[0] - i] == PAVE || plateau[lesX[0] + movDroite][lesY[0] + i] == PAVE)
            {
                movDroite++;
            }
        }
    }
    else if (movY == 0 && movX != 0) // Déplacement horizontal
    {
        for (int i = 0; i <= movX; i++)
        {
            if (plateau[lesX[0] - i][lesY[0] - movHaut] == PAVE || plateau[lesX[0] + i][lesY[0] - movHaut] == PAVE)
            {
                movHaut++;
            }
        }
        for (int i = 0; i <= movX; i++)
        {
            if (plateau[lesX[0] - i][lesY[0] + movBas] == PAVE || plateau[lesX[0] + i][lesY[0] + movBas] == PAVE)
            {
                movBas++;
            }
        }
    }
    if (movX != 0 && movY != 0)
    {
        if (*derniereDirection != DROITE)
        {
            for (int i = 0; i <= movY; i++)
            {
                if (plateau[lesX[0] - movGauche][lesY[0] - i] == PAVE || plateau[lesX[0] - movGauche][lesY[0] + i] == PAVE)
                {
                    movGauche++;
                }
            }
        }
        if (*derniereDirection != GAUCHE)
        {
            for (int i = 0; i <= movY; i++)
            {
                if (plateau[lesX[0] + movDroite][lesY[0] - i] == PAVE || plateau[lesX[0] + movDroite][lesY[0] + i] == PAVE)
                {
                    movDroite++;
                }
            }
        }
        if (*derniereDirection != BAS)
        {
            for (int i = 0; i <= movX; i++)
            {
                if (plateau[lesX[0] - i][lesY[0] - movHaut] == PAVE || plateau[lesX[0] + i][lesY[0] - movHaut] == PAVE)
                {
                    movHaut++;
                }
            }
        }
        if (*derniereDirection != HAUT)
        {
            for (int i = 0; i <= movX; i++)
            {
                if (plateau[lesX[0] - i][lesY[0] + movBas] == PAVE || plateau[lesX[0] + i][lesY[0] + movBas] == PAVE)
                {
                    movBas++;
                }
            }
        }
    }
    gotoxy(1, HAUTEUR_PLATEAU + 5);
    printf("ici movGauche %d, movDroite %d, movHaut %d, movBas %d\n", movGauche, movDroite, movHaut, movBas);

    // Choix de la direction optimale
    if (movGauche < movDroite || movDroite == 0)
    {
        for (int i = 0; i < movGauche; i++)
        {
            progresser(lesX, lesY, GAUCHE);
            *derniereDirection = GAUCHE;
        }
        *axeEscive = 0;
    }
    else
    {
        for (int i = 0; i < movDroite; i++)
        {
            progresser(lesX, lesY, DROITE);
            *derniereDirection = DROITE;
        }
        *axeEscive = 0;
    }
    if (movBas < movHaut || movHaut == 0)
    {
        for (int i = 0; i < movBas; i++)
        {
            progresser(lesX, lesY, BAS);
            *derniereDirection = BAS;
        }
        *axeEscive = 1;
    }
    else
    {
        for (int i = 0; i < movHaut; i++)
        {
            progresser(lesX, lesY, HAUT);
            *derniereDirection = HAUT;
        }
        *axeEscive = 1;
    }
}
