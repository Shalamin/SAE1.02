//
// Created by KERAUDREN johan on 18/12/2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

// taille du serpent
#define TAILLE 10
// dimensions du plateau
#define LARGEUR_PLATEAU 80
#define HAUTEUR_PLATEAU 40
// position initiale de la tête du serpent
#define X_INITIAL 40
#define Y_INITIAL 20
// nombre de pommes à manger pour gagner
#define NB_POMMES 10
// temporisation entre deux déplacements du serpent (en microsecondes)
#define ATTENTE 200000
// caractères pour représenter le serpent
#define CORPS 'X'
#define TETE 'O'
// touches de direction ou d'arrêt du jeu
#define HAUT 'z'
#define BAS 's'
#define GAUCHE 'q'
#define DROITE 'd'
#define STOP 'a'
// caractères pour les éléments du plateau
#define BORDURE '#'
#define VIDE ' '
#define POMME '6'
// numerotation des portail
#define P_HAUT 1
#define P_GAUCHE 2
#define P_DROITE 3
#define P_BAS 4

// définition d'un type pour le plateau : tPlateau
// Attention, pour que les indices du tableau 2D (qui commencent à 0) coincident
// avec les coordonées à l'écran (qui commencent à 1), on ajoute 1 aux dimensions
// et on neutralise la ligne 0 et la colonne 0 du tableau 2D (elles ne sont jamais
// utilisées)
typedef char tPlateau[LARGEUR_PLATEAU + 1][HAUTEUR_PLATEAU + 1];
int lesPommesX[NB_POMMES] = {75, 75, 78, 2, 8, 78, 74, 2, 72, 5};
int lesPommesY[NB_POMMES] = {8, 39, 2, 2, 5, 39, 33, 38, 35, 2};

void initPlateau(tPlateau plateau);
void dessinerPlateau(tPlateau plateau);
void ajouterPomme(tPlateau plateau, int *xPomme, int *yPomme, int nbPommes);
void afficher(int, int, char);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void calculDistance(int positionX, int lesX[], int positionY, int lesY[], int *movX, int *movY);
void definirDirection(char *direction, char *lastDirection, int movX, int movY, int lesX[], int lesY[]);
int valAbsolu(int valeur);
void vaVersPortail(int portail, int lesX[], int lesY[], int *distancePortailX, int *distancePortailY);
void choixPortail(int lesX[], int lesY[], int xPomme, int yPomme, int *portail, int *distancePommeX, int *distancePommeY, int *distancePortailX, int *distancePortailY);
void progresser(int lesX[], int lesY[], char direction);
void gestionCollisions(int lesX[], int lesY[], tPlateau plateau, bool *collision, bool *pomme);
void finDuJeu(int numeroPomme, time_t debut_t, time_t fin_t);
void gotoxy(int x, int y);
int kbhit();
void disableEcho();
void enableEcho();

int main()
{
    // 2 tableaux contenant les positions des éléments qui constituent le serpent
    int lesX[TAILLE];
    int lesY[TAILLE];

    // représente la touche frappée par l'utilisateur : touche de direction ou pour l'arrêt
    char touche;

    // direction courante du serpent (HAUT, BAS, GAUCHE ou DROITE)
    char direction;
    char lastDirection;

    // le plateau de jeu
    tPlateau lePlateau;

    bool collision = false;
    bool gagne = false;
    bool pommeMangee = false;
    int xPomme, yPomme;
    int deplacement = 0;
    clock_t debut_t, fin_t;
    // compteur de pommes mangées
    int nbPommes = 0;
    // stock si le serpent doit prendre un portail et lequel , est a 0 sinon;
    int portail = 0;
    // distance avec la pomme
    int distancePommeX, distancePommeY;
    // distance le portail a prendre
    int distancePortailX, distancePortailY;
    // nombre de mouvement dans une même direction
    int nbMovement;

    // initialisation de la position du serpent : positionnement de la
    // tête en (X_INITIAL, Y_INITIAL), puis des anneaux à sa gauche

    for (int i = 0; i < TAILLE; i++)
    {
        lesX[i] = X_INITIAL - i;
        lesY[i] = Y_INITIAL;
    }
    // mise en place du plateau
    initPlateau(lePlateau);
    system("clear");
    dessinerPlateau(lePlateau);

    srand(time(NULL));
    ajouterPomme(lePlateau, &xPomme, &yPomme, nbPommes);

    // initialisation : le serpent se dirige vers la DROITE
    dessinerSerpent(lesX, lesY);
    disableEcho();
    direction = DROITE;
    lastDirection = DROITE;
    touche = DROITE;

    // boucle de jeu. Arret si touche STOP, si collision avec une bordure ou
    // si toutes les pommes sont mangées
    debut_t = clock();
    do
    {
        choixPortail(lesX, lesY, xPomme, yPomme, &portail, &distancePommeX, &distancePommeY, &distancePortailX, &distancePortailY);
        if (portail > 0)
        {
            printf("toto");
            // traite le déplacement vers le portail en X
            if (distancePortailX != 0)
            {
                definirDirection(&direction, &lastDirection, distancePortailX, distancePortailY, lesX, lesY);
                nbMovement = valAbsolu(distancePortailX);
                deplacement += nbMovement;
                for (int i = 0; i < nbMovement && touche != STOP && !collision && !pommeMangee; i++)
                {
                    progresser(lesX, lesY, direction);
                    gestionCollisions(lesX, lesY, lePlateau, &collision, &pommeMangee);
                    if (!gagne)
                    {
                        if (!collision)
                        {
                            usleep(ATTENTE);
                            if (kbhit() == 1)
                            {
                                touche = getchar();
                            }
                        }
                    }
                }
                distancePortailX = 0;
            }
            // traite le déplacement vers le portail en Y
            if (distancePortailY != 0)
            {
                definirDirection(&direction, &lastDirection, distancePortailX, distancePortailY, lesX, lesY);

                nbMovement = valAbsolu(distancePortailY);
                deplacement += nbMovement;
                for (int i = 0; i < nbMovement && touche != STOP && !collision && !pommeMangee; i++)
                {
                    progresser(lesX, lesY, direction);
                    gestionCollisions(lesX, lesY, lePlateau, &collision, &pommeMangee);
                    if (!gagne)
                    {
                        if (!collision)
                        {
                            usleep(ATTENTE);
                            if (kbhit() == 1)
                            {
                                touche = getchar();
                            }
                        }
                    }
                }
                distancePortailY = 0;
            }
        }
        // traite le déplacement vers la pomme en X
        if (distancePommeX != 0)
        {
          	definirDirection(&direction, &lastDirection, distancePommeX, distancePommeY, lesX, lesY);
            printf("X : %d direction : %c", distancePommeX, direction);
            nbMovement = valAbsolu(distancePommeX);
            deplacement += nbMovement;
            for (int i = 0; i < nbMovement && touche != STOP && !collision && !pommeMangee; i++)
            {
                progresser(lesX, lesY, direction);
                gestionCollisions(lesX, lesY, lePlateau, &collision, &pommeMangee);
                if (!gagne)
                {
                    if (!collision)
                    {
                        usleep(ATTENTE);
                        if (kbhit() == 1)
                        {
                            touche = getchar();
                        }
                    }
                }
            }
            distancePommeX = 0;
        }
        // traite le déplacement vers la pomme en Y
        if (distancePommeY != 0)
        {
            definirDirection(&direction, &lastDirection, distancePommeX, distancePommeY, lesX, lesY);
            printf("Y : %d direction : %c", distancePommeY, direction);
            nbMovement = valAbsolu(distancePommeY);
            deplacement += nbMovement;
            for (int i = 0; i < nbMovement && touche != STOP && !collision&& !pommeMangee; i++)
            {
                progresser(lesX, lesY, direction);
                gestionCollisions(lesX, lesY, lePlateau, &collision, &pommeMangee);
                if (!gagne)
                {
                    if (!collision)
                    {
                        usleep(ATTENTE);
                        if (kbhit() == 1)
                        {
                            touche = getchar();
                        }
                    }
                }
            }
            distancePommeY = 0;
        }
        if (pommeMangee)
        {
            nbPommes++;
            gagne = (nbPommes == NB_POMMES);
            if (!gagne)
            {
                ajouterPomme(lePlateau, &xPomme, &yPomme, nbPommes);
                pommeMangee = false;
            }
        }
    } while (touche != STOP && !collision && !gagne);
    fin_t = clock();
    enableEcho();
    finDuJeu(nbPommes, debut_t, fin_t);
    return EXIT_SUCCESS;
}

/************************************************/
/*		FONCTIONS ET PROCEDURES DU JEU 			*/
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

void ajouterPomme(tPlateau plateau, int *xPomme, int *yPomme, int nbPommes)
{
    // génère aléatoirement la position d'une pomme,
    // vérifie que ça correspond à une case vide
    // du plateau puis l'ajoute au plateau et l'affiche
    *xPomme = lesPommesX[nbPommes];
    *yPomme = lesPommesY[nbPommes];
    plateau[*xPomme][*yPomme] = POMME;
    afficher(*xPomme, *yPomme, POMME);
}

void afficher(int x, int y, char car)
{
    gotoxy(x, y);
    printf("%c", car);
    gotoxy(1, 1);
}

void effacer(int x, int y)
{
    gotoxy(x, y);
    printf(" ");
    gotoxy(1, 1);
}

void dessinerSerpent(int lesX[], int lesY[])
{
    // affiche les anneaux puis la tête
    for (int i = 1; i < TAILLE; i++)
    {
        afficher(lesX[i], lesY[i], CORPS);
    }
    afficher(lesX[0], lesY[0], TETE);
}

void calculDistance(int positionX, int lesX[], int positionY, int lesY[], int *movX, int *movY)
{
    // calcul la différence des possition entre une case  de position positionX et positionY et la tête du serpent
    *movX = positionX - lesX[0];
    *movY = positionY - lesY[0];
}
void demiTour(int lesX[], int lesY[], char directionDT){
    char directionDemiTour;
    switch (directionDT)
    {
    case HAUT:

        if ((lesX[0] + 1) != LARGEUR_PLATEAU)
        {
            directionDemiTour = DROITE;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = BAS;
            for (int i = 0; i < 4; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
        }
        else
        {
            directionDemiTour = GAUCHE;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = BAS;
            for (int i = 0; i < 4; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
        }
        break;
    case BAS:
        if ((lesX[0] + 1) != LARGEUR_PLATEAU)
        {
            directionDemiTour = DROITE;
            progresser(lesX, lesY, directionDemiTour);
        }
        else
        {
            directionDemiTour = GAUCHE;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = HAUT;
        }
        break;
    case DROITE:
        if ((lesY[0] - 1) != 1)
        {
            directionDemiTour = HAUT;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = GAUCHE;
            for (int i = 0; i < 4; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
        }
        else
        {
            directionDemiTour = BAS;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = GAUCHE;
            for (int i = 0; i < 4; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
        }
        break;
    case GAUCHE:
        if ((lesY[0] - 1) != 1)
        {
            directionDemiTour = HAUT;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = DROITE;
            for (int i = 0; i < 4; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
        }
        else
        {
            directionDemiTour = BAS;
            progresser(lesX, lesY, directionDemiTour);
            directionDemiTour = DROITE;
            for (int i = 0; i < 4; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
            break;
        }
    }
}
void definirDirection(char *direction, char *lastDirection, int movX, int movY, int lesX[], int lesY[])
{
    // @brief Définit la direction du serpent
    // @param direction : la direction actuelle du serpent
    // @param movX : le mouvement en x
    // @param movY : le mouvement en y
    // @return la direction du serpent

    if (movX == 0)

    {
        if (movY > 0)
            if (*lastDirection == HAUT)
            {
                demiTour(lesX, lesY, *direction);
                *direction = BAS;
            }
            else
            {
                *direction = BAS;
            }
        else
        {
            if (*lastDirection == BAS)
            {
                demiTour(lesX, lesY, *direction);
                *direction = HAUT;
            }
            else
            {
                *direction = HAUT;
            }
        }
    }
    else
    {
        if (movX < 0)
            if (*lastDirection == DROITE)
            {
                demiTour(lesX, lesY, *direction);
                *direction = GAUCHE;
            }
            else
            {
                *direction = GAUCHE;
            }
        else
        {
            if (*lastDirection == GAUCHE)
            {
                demiTour(lesX, lesY, *direction);
                *direction = DROITE;
            }
            else
            {
                *direction = DROITE;
            }
        }
    }
    *lastDirection = *direction;
}

int valAbsolu(int valeur)
{
    return (valeur < 0) ? -valeur : valeur;
}

void vaVersPortail(int portail, int lesX[], int lesY[], int *distancePortailX, int *distancePortailY)
{
    // calcul et renvoie la ditance avec le portail souhaiter
    if (portail == P_HAUT)
    {
        calculDistance((LARGEUR_PLATEAU / 2), lesX, 1, lesY, &(*distancePortailX), &(*distancePortailY));
    }
    else if (portail == P_GAUCHE)
    {
        calculDistance(1, lesX, (HAUTEUR_PLATEAU / 2), lesY, &(*distancePortailX), &(*distancePortailY));
    }
    else if (portail == P_DROITE)
    {
        calculDistance(LARGEUR_PLATEAU, lesX, (HAUTEUR_PLATEAU / 2), lesY, &(*distancePortailX), &(*distancePortailY));
    }
    else
    {
        calculDistance((LARGEUR_PLATEAU / 2), lesX, HAUTEUR_PLATEAU, lesY, &(*distancePortailX), &(*distancePortailY));
    }
}

void choixPortail(int lesX[], int lesY[], int xPomme, int yPomme, int *portail,int *distancePommeX,int *distancePommeY, int *distancePortailX, int *distancePortailY)
{
    //choisie si il faut prendre un portail et quel portail prendre
    //et calcule renvoie la valeur des distance du portail et de la pomme une fois sortie du portail

    int distancePortail1X, distancePortail1Y, distancePortail2X, distancePortail2Y, nbMov1, nbMov2, nbMov3;
    int distancePommeAvecPortail1X, distancePommeAvecPortail1Y, distancePommeAvecPortail2X, distancePommeAvecPortail2Y;

    *distancePortailX = 0;
    *distancePortailY = 0;

    calculDistance(xPomme, lesX, yPomme, lesY, &(*distancePommeX), &(*distancePommeY));

    //cherche si il faut prendre un portail quand la pomme sur la gauche
    if(xPomme < LARGEUR_PLATEAU/2)
    {
        //cherche si il faut prendre un portail quand la pomme est en haut à gauche
        if(yPomme < HAUTEUR_PLATEAU/2)
        {
            //calcul la distance jusqu'au portail
            vaVersPortail(P_BAS, lesX, lesY, &distancePortail1X, &distancePortail1Y);
            vaVersPortail(P_DROITE, lesX, lesY, &distancePortail2X, &distancePortail2Y);
            //calcul la distance a la pomme en prenant un portail
            distancePommeAvecPortail1X = valAbsolu(xPomme-LARGEUR_PLATEAU/2)+ valAbsolu(distancePortail1X);
            distancePommeAvecPortail1Y = valAbsolu(yPomme-1) + valAbsolu(distancePortail1Y);
            distancePommeAvecPortail2X = valAbsolu(xPomme-1) + valAbsolu(distancePortail2X);
            distancePommeAvecPortail2Y = valAbsolu(yPomme-HAUTEUR_PLATEAU/2) + valAbsolu(distancePortail2Y);
            //calcul le nombre de mouvement  pour chacun des trois parcoure
            nbMov1 = distancePommeAvecPortail1X + distancePommeAvecPortail1Y;
            nbMov2 = distancePommeAvecPortail2X + distancePommeAvecPortail2Y;
            nbMov3 = valAbsolu(*distancePommeY) + valAbsolu(*distancePommeY);
            if(nbMov1 < nbMov3)
            {
                if(nbMov1 < nbMov2)
                {
                    *portail = P_BAS;
                    *distancePortailX = distancePortail1X;
                    *distancePortailY = distancePortail1Y + 2;
                    *distancePommeX = *distancePommeX-distancePortail1X;
                    *distancePommeY = yPomme-1;
                }
                else if(nbMov2 < nbMov3)
                {
                    *portail = P_DROITE;
                    *distancePortailX = distancePortail2X + 2;
                    *distancePortailY = distancePortail2Y;
                    *distancePommeX = xPomme-LARGEUR_PLATEAU;
                    *distancePommeY = distancePommeAvecPortail2Y;
                }
                else
                {
                    *portail = 0;
                }
            }
            else if(nbMov2 < nbMov3)
            {
                *portail = P_DROITE;
                *distancePortailX = distancePortail2X + 2;
                *distancePortailY = distancePortail2Y;
                *distancePommeX = xPomme-LARGEUR_PLATEAU;
                *distancePommeY = *distancePommeY-distancePortail2Y;
            }
            else
            {
                *portail = 0;
            }
        }
        //cherche si il faut prendre un portail quand la pomme est en bas à gauche
        else
        {
            //calcul la distance jusqu'au portail
            vaVersPortail(P_HAUT, lesX, lesY, &distancePortail1X, &distancePortail1Y);
            vaVersPortail(P_DROITE, lesX, lesY, &distancePortail2X, &distancePortail2Y);
            //calcul la distance a la pomme en prenent un portail
            distancePommeAvecPortail1X = valAbsolu(xPomme-LARGEUR_PLATEAU/2)+ valAbsolu(distancePortail1X);
            distancePommeAvecPortail1Y = valAbsolu(yPomme-HAUTEUR_PLATEAU) + valAbsolu(distancePortail1Y);
            distancePommeAvecPortail2X = valAbsolu(xPomme-1) + valAbsolu(distancePortail2X);
            distancePommeAvecPortail2Y = valAbsolu(yPomme-HAUTEUR_PLATEAU/2) + valAbsolu(distancePortail2Y);
            //calcul le nombre de mouvement  pour chacun des trois parcoure
            nbMov1 = distancePommeAvecPortail1X + distancePommeAvecPortail1Y;
            nbMov2 = distancePommeAvecPortail2X + distancePommeAvecPortail2Y;
            nbMov3 = valAbsolu(*distancePommeX) + valAbsolu(*distancePommeY);
            if(nbMov1 < nbMov3)
            {
                if(nbMov1 < nbMov2)
                {
                    *portail = P_HAUT;
                    *distancePortailX = distancePortail1X;
                    *distancePortailY = distancePortail1Y - 2;
                    *distancePommeX = *distancePommeX-distancePortail1X;
                    *distancePommeY = yPomme-HAUTEUR_PLATEAU;
                }
                else if(nbMov2 < nbMov3)
                {
                    *portail = P_DROITE;
                    *distancePortailX = distancePortail2X + 2;
                    *distancePortailY = distancePortail2Y;
                    *distancePommeX = xPomme-LARGEUR_PLATEAU;
                    *distancePommeY = distancePommeAvecPortail2Y;
                }
                else
                {
                    *portail = 0;
                }
            }
            else if(nbMov2 < nbMov3)
            {
                *portail = P_DROITE;
                *distancePortailX = distancePortail2X + 2;
                *distancePortailY = distancePortail2Y;
                *distancePommeX = xPomme-LARGEUR_PLATEAU;
                *distancePommeY = *distancePommeY-distancePortail2Y;
            }
            else
            {
                *portail = 0;
            }
        }
    }
    //cherche si il faut prendre un portail quand la pomme sur la droite
    else
    {
        //cherche si il faut prendre un portail quand la pomme est en haut à droite
        if(yPomme < HAUTEUR_PLATEAU/2)
        {
            //calcul la distance jusqu'au portail
            vaVersPortail(P_BAS, lesX, lesY, &distancePortail1X, &distancePortail1Y);
            vaVersPortail(P_GAUCHE, lesX, lesY, &distancePortail2X, &distancePortail2Y);
            //calcul la distance a la pomme en prenent un portail
            distancePommeAvecPortail1X = valAbsolu(xPomme-LARGEUR_PLATEAU/2)+ valAbsolu(distancePortail1X);
            distancePommeAvecPortail1Y = valAbsolu(yPomme-1) + valAbsolu(distancePortail1Y);
            distancePommeAvecPortail2X = valAbsolu(xPomme-LARGEUR_PLATEAU) + valAbsolu(distancePortail2X);
            distancePommeAvecPortail2Y = valAbsolu(yPomme-HAUTEUR_PLATEAU/2) + valAbsolu(distancePortail2Y);
            //calcul le nombre de mouvement  pour chacun des trois parcoure
            nbMov1 = distancePommeAvecPortail1X + distancePommeAvecPortail1Y;
            nbMov2 = distancePommeAvecPortail2X + distancePommeAvecPortail2Y;
            nbMov3 = valAbsolu(*distancePommeX) + valAbsolu(*distancePommeY);
            if(nbMov1 < nbMov3)
            {
                if(nbMov1 < nbMov2)
                {
                    *portail = P_BAS;
                    *distancePortailX = distancePortail1X;
                    *distancePortailY = distancePortail1Y + 2;
                    *distancePommeX = *distancePommeX-distancePortail1X;
                    *distancePommeY = yPomme-1;
                }
                else if(nbMov2 < nbMov3)
                {
                    *portail = P_GAUCHE;
                    *distancePortailX = distancePortail2X - 2;
                    *distancePortailY = distancePortail2Y;
                    *distancePommeX = xPomme-1;
                    *distancePommeY = *distancePommeY-distancePortail2Y;
                }
                else
                {
                    *portail = 0;
                }
            }
            else if(nbMov2 < nbMov3)
            {
                *portail = P_GAUCHE;
                *distancePortailX = distancePortail2X - 2;
                *distancePortailY = distancePortail2Y;
                *distancePommeX = xPomme-1;
                *distancePommeY = *distancePommeY-distancePortail2Y;
            }
            else
            {
                *portail = 0;
            }
        }
        //cherche si il faut prendre un portail quand la pomme est en bas à droite
        else
        {
            //calcul la distance jusqu'au portail
            vaVersPortail(P_HAUT, lesX, lesY, &distancePortail1X, &distancePortail1Y);
            vaVersPortail(P_GAUCHE, lesX, lesY, &distancePortail2X, &distancePortail2Y);
            //calcul la distance a la pomme en prenent un portail
            distancePommeAvecPortail1X = valAbsolu(xPomme-LARGEUR_PLATEAU/2)+ valAbsolu(distancePortail1X);
            distancePommeAvecPortail1Y = valAbsolu(yPomme-HAUTEUR_PLATEAU) + valAbsolu(distancePortail1Y);
            distancePommeAvecPortail2X = valAbsolu(xPomme-LARGEUR_PLATEAU) + valAbsolu(distancePortail2X);
            distancePommeAvecPortail2Y = valAbsolu(yPomme-HAUTEUR_PLATEAU/2) + valAbsolu(distancePortail2Y);
            //calcul le nombre de mouvement  pour chacun des trois parcoure
            nbMov1 = distancePommeAvecPortail1X + distancePommeAvecPortail1Y;
            nbMov2 = distancePommeAvecPortail2X + distancePommeAvecPortail2Y;
            nbMov3 = valAbsolu(*distancePommeX) + valAbsolu(*distancePommeY);
            if(nbMov1 < nbMov3)
            {
                if(nbMov1 < nbMov2)
                {
                    *portail = P_HAUT;
                    *distancePortailX = distancePortail1X;
                    *distancePortailY = distancePortail1Y - 2;
                    *distancePommeX = *distancePommeX-distancePortail1X;
                    *distancePommeY = yPomme-HAUTEUR_PLATEAU;
                }
                else if(nbMov2 < nbMov3)
                {
                    *portail = P_GAUCHE;
                    *distancePortailX = distancePortail2X - 2;
                    *distancePortailY = distancePortail2Y;
                    *distancePommeX = -(*distancePommeX-distancePortail2X)%LARGEUR_PLATEAU;
                    *distancePommeY = *distancePommeY-distancePortail2Y;
                }
                else
                {
                    *portail = 0;
                }
            }
            else if(nbMov2 < nbMov3)
            {
                *portail = P_GAUCHE;
                *distancePortailX = distancePortail2X - 2;
                *distancePortailY = distancePortail2Y;
                *distancePommeX = -(*distancePommeX-distancePortail2X)%LARGEUR_PLATEAU;
                *distancePommeY = *distancePommeY-distancePortail2Y;
            }
            else
            {
                *portail = 0;
            }
        }
    }
}

void progresser(int lesX[], int lesY[], char direction)
{
    // efface le dernier élément avant d'actualiser la position de tous les
    // élémentds du serpent avant de le  redessiner et détecte une
    // collision avec une pomme ou avec une bordure
    effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);

    for (int i = TAILLE - 1; i > 0; i--)
    {
        lesX[i] = lesX[i - 1];
        lesY[i] = lesY[i - 1];
    }
    // faire progresser la tete dans la nouvelle direction en prenant en conte les portail
    switch (direction)
    {
    case HAUT:
        lesY[0] = (lesY[0] - 1) % HAUTEUR_PLATEAU;
        break;
    case BAS:
        lesY[0] = (lesY[0] + 1) % HAUTEUR_PLATEAU;
        break;
    case DROITE:
        lesX[0] = (lesX[0] + 1) % LARGEUR_PLATEAU;
        break;
    case GAUCHE:
        lesX[0] = (lesX[0] - 1) % LARGEUR_PLATEAU;
        break;
    }
    // dessine le serpent
    dessinerSerpent(lesX, lesY);
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
                *collision = true;
        }
    }
}

void finDuJeu(int numeroPomme, time_t debut_t, time_t fin_t)
{
    /* @brief Fin du programme , message de fin et réactivation de l'écriture dans la console*/
    enableEcho();
    gotoxy(1, HAUTEUR_PLATEAU + 1);
    double total = ((double)(fin_t - debut_t)) / CLOCKS_PER_SEC;

    printf("La partie est terminée !\n");
    printf("Votre score est de ; %d\n", numeroPomme);
    printf("Le temps total CPU à été de %f secondes\n", total);
}

/************************************************/
/*				 FONCTIONS UTILITAIRES 			*/
/************************************************/
void gotoxy(int x, int y)
{
    printf("\033[%d;%df", y, x);
}

int kbhit()
{
    // la fonction retourne :
    // 1 si un caractere est present
    // 0 si pas de caractere présent
    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    // mettre le terminal en mode non bloquant
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    // restaurer le mode du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    }
    return unCaractere;
}

// Fonction pour désactiver l'echo
void disableEcho()
{
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Désactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// Fonction pour réactiver l'echo
void enableEcho()
{
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1)
    {
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Réactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
    {
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}
