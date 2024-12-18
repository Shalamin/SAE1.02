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
int lesPommesX[NB_POMMES] = {40, 40, 40, 2, 8, 78, 74, 2, 72, 5};
int lesPommesY[NB_POMMES] = {39, 2, 39, 2, 5, 39, 33, 38, 35, 2};

void initPlateau(tPlateau plateau);
void dessinerPlateau(tPlateau plateau);
void ajouterPomme(tPlateau plateau, int *xPomme, int *yPomme, int nbPommes);
void afficher(int, int, char);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void demiTour(int lesX[], int lesY[], char directionDT,int *mov);
void definirDirection(char *direction, char *lastDirection, int *movX, int *movY,int ordreDeplacement, int lesX[], int lesY[]);
int valAbsolu(int valeur);
int calculDistance(int position1, int position2);
void choixPortail(int lesX[], int lesY[], int xPomme, int yPomme, int *portail, int *distancePommeX, int *distancePommeY, int *distancePortailX, int *distancePortailY);
void progresser(int lesX[], int lesY[], char direction);
void gestionCollisions(int lesX[], int lesY[], tPlateau plateau, bool *collision, bool *pomme);
void finDuJeu(int numeroPomme, time_t debut_t, time_t fin_t, int nbDeplacement);
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
    //nombre de déplacement total
    int nbDeplacement = 0;
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
    // nombre qui permet de l'odre de déplacement du serpent en X puit en Y ou invercement
    int ordreDeplacement = 0;
    //nombre permettant de géré les boucle while du main
    int i = 0;


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
        if(portail == 0)
        {
            if(lastDirection == HAUT || lastDirection == BAS)
            {
                ordreDeplacement = 2;
            }
            else
            {
                ordreDeplacement = 3;
            }
        }
        else
        {
            if(portail == P_HAUT || portail == P_BAS)
            {
                if(distancePortailX != 0)
                {
                    ordreDeplacement = 0;
                }
                else
                {
                    ordreDeplacement = 1;
                }
            }
            else
            {
                if(distancePortailY != 0)
                {
                    ordreDeplacement = 1;
                }
                else
                {
                    ordreDeplacement = 0;
                }
            }
        }
        // traite le déplacement vers le portail en X
        if (distancePortailX != 0 && ordreDeplacement == 0)
        {
            definirDirection(&direction, &lastDirection, &distancePortailX, &distancePortailY, ordreDeplacement, lesX, lesY);
            nbMovement = valAbsolu(distancePortailX);
            nbDeplacement += nbMovement;
            while(i++ < nbMovement && touche != STOP && !collision && !pommeMangee)
            {
                progresser(lesX, lesY, direction);
                gestionCollisions(lesX, lesY, lePlateau, &collision, &pommeMangee);
                if (!gagne)
                {
                    if (!collision)
                    {
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
        else if (distancePortailY != 0 && ordreDeplacement == 1)
        {
            definirDirection(&direction, &lastDirection, &distancePortailX, &distancePortailY, ordreDeplacement, lesX, lesY);
            nbMovement = valAbsolu(distancePortailY);
            nbDeplacement += nbMovement;
            while(i++ < nbMovement && touche != STOP && !collision && !pommeMangee)
            {
                progresser(lesX, lesY, direction);
                gestionCollisions(lesX, lesY, lePlateau, &collision, &pommeMangee);
                if (!gagne)
                {
                    if (!collision)
                    {
                        if (kbhit() == 1)
                        {
                            touche = getchar();
                        }
                    }
                }
            }
            distancePortailY = 0;
        }
        // traite le déplacement vers la pomme en X
        else if (distancePommeX != 0 && ordreDeplacement == 2)
        {
            definirDirection(&direction, &lastDirection, &distancePommeX, &distancePommeY, ordreDeplacement, lesX, lesY);
            nbMovement = valAbsolu(distancePommeX);
            nbDeplacement += nbMovement;
            while(i++ < nbMovement && touche != STOP && !collision && !pommeMangee)
            {
                progresser(lesX, lesY, direction);
                gestionCollisions(lesX, lesY, lePlateau, &collision, &pommeMangee);
                if (!gagne)
                {
                    if (!collision)
                    {
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
        else if (distancePommeY != 0 && ordreDeplacement == 3)
        {
            definirDirection(&direction, &lastDirection, &distancePommeX, &distancePommeY, ordreDeplacement, lesX, lesY);
            nbMovement = valAbsolu(distancePommeY);
            nbDeplacement += nbMovement;
            while(i++ < nbMovement && touche != STOP && !collision && !pommeMangee)
            {
                progresser(lesX, lesY, direction);
                gestionCollisions(lesX, lesY, lePlateau, &collision, &pommeMangee);
                if (!gagne)
                {
                    if (!collision)
                    {
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
        i = 0;
    } while (touche != STOP && !collision && !gagne);
    fin_t = clock();
    enableEcho();
    finDuJeu(nbPommes, debut_t, fin_t, nbDeplacement);
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
                            ?   BORDURE : VIDE;
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

void demiTour(int lesX[], int lesY[], char directionDT,int *mov)
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
            for (int i = 0; i < (TAILLE/2)-1; i++)
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
            for (int i = 0; i < (TAILLE/2)-1; i++)
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
            for (int i = 0; i < (TAILLE/2)-1; i++)
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
            for (int i = 0; i < (TAILLE/2)-1; i++)
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
            for (int i = 0; i < (TAILLE/2)-1; i++)
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
            for (int i = 0; i < (TAILLE/2)-1; i++)
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
            for (int i = 0; i < (TAILLE/2)-1; i++)
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
            for (int i = 0; i < (TAILLE/2)-1; i++)
            {
                progresser(lesX, lesY, directionDemiTour);
            }
            (*mov)--;
        }
    }
}
void definirDirection(char *direction, char *lastDirection, int *movX, int *movY,int ordreDeplacement, int lesX[], int lesY[])
{
    if(ordreDeplacement == 0 || ordreDeplacement == 2)
    {
        if(*lastDirection == DROITE)
        {
            if(*movX < 0)
            {
                demiTour(lesX, lesY, GAUCHE, &*movY);
                *movX+=(TAILLE/2)-1;
                *direction = GAUCHE;
            }
            else
            {
                *direction = DROITE;
            }
            
        }
        else if(*lastDirection == GAUCHE)
        {
            if(*movX > 0)
            {
                demiTour(lesX, lesY, DROITE, &*movY);
                *movX-=(TAILLE/2)-1;
                *direction = DROITE;
            }
            else
            {
                *direction = GAUCHE;
            }
        }
        else
        {
            *direction = (*movX > 0) ?   DROITE : GAUCHE;
        }
    }
    else
    {
        if(*lastDirection == BAS)
        {
            if(*movY < 0)
            {
                demiTour(lesX, lesY, HAUT, &*movX);
                *movY+=(TAILLE/2)-1;
                *direction = HAUT;
            }
            else
            {
                *direction = BAS;
            }
            
        }
        else if(*lastDirection == HAUT)
        {
            if(*movY > 0)
            {
                demiTour(lesX, lesY, BAS, &*movX);
                *movY-=(TAILLE/2)-1;
                *direction = BAS;
            }
            else
            {
                *direction = HAUT;
            }
        }
        else
        {
            *direction = (*movY > 0) ?   BAS : HAUT;
        }
    }
    *lastDirection = *direction;
}

int valAbsolu(int valeur)
{
    return (valeur < 0) ? -valeur : valeur;
}

int calculDistance(int position1, int position2)
{
    //calcul la différence entre deux position
    return position1 - position2;
}

void choixPortail(int lesX[], int lesY[], int xPomme, int yPomme, int *portail,int *distancePommeX,int *distancePommeY, int *distancePortailX, int *distancePortailY)
{
    //choisie si il faut prendre un portail et quel portail prendre
    //et calcule renvoie la valeur des distance du portail et de la pomme une fois sortie du portail
    int choixPortail1, choixPortail2, nonPortail;
    if(xPomme < LARGEUR_PLATEAU/2)
    {
        if(yPomme < HAUTEUR_PLATEAU/2)
        {
            //La pomme est sur la partie en haut à gauche du plateau

            //addition le nombre de mouvemant pour allez jusqu'à la pomme pour les différent choix

            //portail en bas
            choixPortail1 = valAbsolu(calculDistance(LARGEUR_PLATEAU/2, lesX[0]))
                          + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU/2))
                          + valAbsolu(calculDistance(HAUTEUR_PLATEAU, lesY[0]))
                          + valAbsolu(calculDistance(yPomme, 1));

            //portail à droite
            choixPortail2 = valAbsolu(calculDistance(LARGEUR_PLATEAU, lesX[0]))
                          + valAbsolu(calculDistance(xPomme, 1))
                          + valAbsolu(calculDistance(HAUTEUR_PLATEAU/2, lesY[0]))
                          + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU/2));

            nonPortail = valAbsolu(calculDistance(xPomme, lesX[0]))
                       + valAbsolu(calculDistance(yPomme, lesY[0]));

            //compare et attribue le parcure a faire en fonction des résultat
            if(choixPortail1 < nonPortail)
            {
                if(choixPortail1 < choixPortail2)
                {
                    *portail = P_BAS;
                    *distancePortailX = calculDistance(LARGEUR_PLATEAU/2, lesX[0]);
                    *distancePortailY = calculDistance(HAUTEUR_PLATEAU, lesY[0]) + 1;
                    *distancePommeX = calculDistance(xPomme, LARGEUR_PLATEAU/2);
                    *distancePommeY = calculDistance(yPomme, 1);
                }
                else
                {
                    *portail = P_DROITE;
                    *distancePortailX = calculDistance(LARGEUR_PLATEAU, lesX[0])  + 1;
                    *distancePortailY = calculDistance(HAUTEUR_PLATEAU/2, lesY[0]);
                    *distancePommeX = calculDistance(xPomme, 1);
                    *distancePommeY = calculDistance(yPomme, HAUTEUR_PLATEAU/2);
                }
            }
            else if(choixPortail2 < nonPortail)
            {
                *portail = P_DROITE;
                *distancePortailX = calculDistance(LARGEUR_PLATEAU, lesX[0]) + 1;
                *distancePortailY = calculDistance(HAUTEUR_PLATEAU/2, lesY[0]);
                *distancePommeX = calculDistance(xPomme, 1);
                *distancePommeY = calculDistance(yPomme, HAUTEUR_PLATEAU/2);
            }
            else
            {
                *portail = 0;
                *distancePommeX = calculDistance(xPomme, lesX[0]);
                *distancePommeY = calculDistance(yPomme, lesY[0]);
            }
        }
        else
        {
            //La pomme est sur la partie en bas à gauche du plateau

            //addition le nombre de mouvemant pour allez jusqu'à la pomme pour les différent choix

            //portail en haut
            choixPortail1 = valAbsolu(calculDistance(LARGEUR_PLATEAU/2, lesX[0]))
                          + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU/2))
                          + valAbsolu(calculDistance(1, lesY[0]))
                          + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU));

            //portail à droite
            choixPortail2 = valAbsolu(calculDistance(LARGEUR_PLATEAU, lesX[0]))
                          + valAbsolu(calculDistance(xPomme, 1))
                          + valAbsolu(calculDistance(HAUTEUR_PLATEAU/2, lesY[0]))
                          + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU/2));

            nonPortail = valAbsolu(calculDistance(xPomme, lesX[0]))
                       + valAbsolu(calculDistance(yPomme, lesY[0]));

            //compare et attribue le parcure a faire en fonction des résultat
            if(choixPortail1 < nonPortail)
            {
                if(choixPortail1 < choixPortail2)
                {
                    *portail = P_HAUT;
                    *distancePortailX = calculDistance(LARGEUR_PLATEAU/2, lesX[0]) - 1;
                    *distancePortailY = calculDistance(1, lesY[0]);
                    *distancePommeX = calculDistance(xPomme, LARGEUR_PLATEAU/2);
                    *distancePommeY = calculDistance(yPomme, HAUTEUR_PLATEAU);
                }
                else
                {
                    *portail = P_DROITE;
                    *distancePortailX = calculDistance(LARGEUR_PLATEAU, lesX[0]) + 1;
                    *distancePortailY = calculDistance(HAUTEUR_PLATEAU/2, lesY[0]);
                    *distancePommeX = calculDistance(xPomme, 1);
                    *distancePommeY = calculDistance(yPomme, HAUTEUR_PLATEAU/2);
                }
            }
            else if(choixPortail2 < nonPortail)
            {
                *portail = P_DROITE;
                *distancePortailX = calculDistance(LARGEUR_PLATEAU, lesX[0]) + 1;
                *distancePortailY = calculDistance(HAUTEUR_PLATEAU/2, lesY[0]);
                *distancePommeX = calculDistance(xPomme, 1);
                *distancePommeY = calculDistance(yPomme, HAUTEUR_PLATEAU/2);
            }
            else
            {
                *portail = 0;
                *distancePommeX = calculDistance(xPomme, lesX[0]);
                *distancePommeY = calculDistance(yPomme, lesY[0]);
            }
        }
    }
    else
    {
        if(yPomme < HAUTEUR_PLATEAU/2)
        {
            //La pomme est sur la partie en haut à droite du plateau

            //addition le nombre de mouvemant pour allez jusqu'à la pomme pour les différent choix

            //portail en bas
            choixPortail1 = valAbsolu(calculDistance(LARGEUR_PLATEAU/2, lesX[0]))
                          + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU/2))
                          + valAbsolu(calculDistance(HAUTEUR_PLATEAU, lesY[0]))
                          + valAbsolu(calculDistance(yPomme, 1));
            //portail à gauche
            choixPortail2 = valAbsolu(calculDistance(1, lesX[0]))
                          + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU))
                          + valAbsolu(calculDistance(HAUTEUR_PLATEAU/2, lesY[0]))
                          + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU/2));

            nonPortail = valAbsolu(calculDistance(xPomme, lesX[0]))
                       + valAbsolu(calculDistance(yPomme, lesY[0]));

            //compare et attribue le parcure a faire en fonction des résultat
            if(choixPortail1 < nonPortail)
            {
                if(choixPortail1 < choixPortail2)
                {
                    *portail = P_BAS;
                    *distancePortailX = calculDistance(LARGEUR_PLATEAU/2, lesX[0]);
                    *distancePortailY = calculDistance(HAUTEUR_PLATEAU, lesY[0]) + 1;
                    *distancePommeX = calculDistance(xPomme, LARGEUR_PLATEAU/2);
                    *distancePommeY = calculDistance(yPomme, 1);
                }
                else
                {
                    *portail = P_GAUCHE;
                    *distancePortailX = calculDistance(1, lesX[0]) - 1;
                    *distancePortailY = calculDistance(HAUTEUR_PLATEAU/2, lesY[0]);
                    *distancePommeX = calculDistance(xPomme, LARGEUR_PLATEAU);
                    *distancePommeY = calculDistance(yPomme, HAUTEUR_PLATEAU/2);
                }
            }
            else if(choixPortail2 < nonPortail)
            {
                *portail = P_GAUCHE;
                *distancePortailX = calculDistance(1, lesX[0]) - 1;
                *distancePortailY = calculDistance(HAUTEUR_PLATEAU/2, lesY[0]);
                *distancePommeX = calculDistance(xPomme, LARGEUR_PLATEAU);
                *distancePommeY = calculDistance(yPomme, HAUTEUR_PLATEAU/2);
            }
            else
            {
                *portail = 0;
                *distancePommeX = calculDistance(xPomme, lesX[0]);
                *distancePommeY = calculDistance(yPomme, lesY[0]);
            }
        }
        else
        {
            //La pomme est sur la partie en haut à froite du plateau

            //addition le nombre de mouvemant pour allez jusqu'à la pomme pour les différent choix

            //portail en haut
            choixPortail1 = valAbsolu(calculDistance(LARGEUR_PLATEAU/2, lesX[0]))
                          + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU/2))
                          + valAbsolu(calculDistance(1, lesY[0]))
                          + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU));

            //portail à gauche
            choixPortail2 = valAbsolu(calculDistance(1, lesX[0]))
                          + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU))
                          + valAbsolu(calculDistance(HAUTEUR_PLATEAU/2, lesY[0]))
                          + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU/2));

            nonPortail = valAbsolu(calculDistance(xPomme, lesX[0]))
                       + valAbsolu(calculDistance(yPomme, lesY[0]));

            if(choixPortail1 < nonPortail)
            {
                if(choixPortail1 < choixPortail2)
                {
                    *portail = P_HAUT;
                    *distancePortailX = calculDistance(LARGEUR_PLATEAU/2, lesX[0]) - 1;
                    *distancePortailY = calculDistance(1, lesY[0]);
                    *distancePommeX = calculDistance(xPomme, LARGEUR_PLATEAU/2);
                    *distancePommeY = calculDistance(yPomme, HAUTEUR_PLATEAU);
                }
                else
                {
                    *portail = P_GAUCHE;
                    *distancePortailX = calculDistance(1, lesX[0]) - 1;
                    *distancePortailY = calculDistance(HAUTEUR_PLATEAU/2, lesY[0]);
                    *distancePommeX = calculDistance(xPomme, LARGEUR_PLATEAU);
                    *distancePommeY = calculDistance(yPomme, HAUTEUR_PLATEAU/2);
                }
            }
            else if(choixPortail2 < nonPortail)
            {
                *portail = P_GAUCHE;
                *distancePortailX = calculDistance(1, lesX[0]) - 1;
                *distancePortailY = calculDistance(HAUTEUR_PLATEAU/2, lesY[0]);
                *distancePommeX = calculDistance(xPomme, LARGEUR_PLATEAU);
                *distancePommeY = calculDistance(yPomme, HAUTEUR_PLATEAU/2);
            }
            else
            {
                *portail = 0;
                *distancePommeX = calculDistance(xPomme, lesX[0]);
                *distancePommeY = calculDistance(yPomme, lesY[0]);
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
    
    //fait anvencer chaque élément de la queue 
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
    if(((lesX[0] == 1 || lesX[0] == LARGEUR_PLATEAU) && lesY[0] == HAUTEUR_PLATEAU/2))
    {
        lesX[0] = valAbsolu(lesX[0]-(LARGEUR_PLATEAU+1));
    }
    else if((lesY[0] == 1 || lesY[0] == HAUTEUR_PLATEAU) && lesX[0] == LARGEUR_PLATEAU/2)
    {
        lesY[0] = valAbsolu(lesY[0]-(HAUTEUR_PLATEAU+1));
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

void finDuJeu(int numeroPomme, time_t debut_t, time_t fin_t, int nbDeplacement)
{
    /* @brief Fin du programme , message de fin et réactivation de l'écriture dans la console*/
    enableEcho();
    gotoxy(1, HAUTEUR_PLATEAU + 1);
    double total = ((double)(fin_t - debut_t)) / CLOCKS_PER_SEC;

    printf("La partie est terminée !\n");
    printf("Votre score est de ; %d\n", numeroPomme);
    printf("Le serpent  a fait %d déplacement\n", nbDeplacement);
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
