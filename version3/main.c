// Inclusion des fichiers d'entête
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
// Inclusion des fichiers d'entête locales
#include "const.h"
#include "types.h"
#include "generation.h"
#include "Utilitaires.h"
#include "jeu.h"
#include "mouvement.h"
#include "portails.h"
#include "direction.h"

// fonction principale

int main()
{
    // 2 tableaux contenant les positions des éléments qui constituent le serpent
    int lesX[TAILLE];
    int lesY[TAILLE];

    // représente la touche frappée par l'utilisateur : touche de direction ou pour l'arrêt
    char touche;

    // direction courante du serpent (HAUT, BAS, GAUCHE ou DROITE)
    char direction;
    char derniereDirection;

    // le plateau de jeu
    tPlateau lePlateau;

    bool collision = false;
    bool gagne = false;
    bool pommeMangee = false;
    int xPomme, yPomme;
    // nombre de déplacement total
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
    // position du portail
    int xPortail, yPortail;
    // nombre de mouvement dans une même direction
    int nbMovement;
    // nombre qui permet de l'odre de déplacement du serpent en X puit en Y ou invercement
    int ordreDeplacement = 0;
    // nombre permettant de géré les boucle while du main
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
    derniereDirection = DROITE;
    touche = DROITE;

    // boucle de jeu. Arret si touche STOP, si collision avec une bordure ou
    // si toutes les pommes sont mangées
    debut_t = clock();
    do
    {
        choixPortail(lesX, lesY, xPomme, yPomme, &portail, &distancePommeX, &distancePommeY, &distancePortailX, &distancePortailY);
        if (portail == P_GAUCHE)
        {
            xPortail = 1;
            yPortail = HAUTEUR_PLATEAU / 2;
        }
        else if (portail == P_HAUT)
        {
            xPortail = LARGEUR_PLATEAU / 2;
            yPortail = 1;
        }
        else if (portail == P_DROITE)
        {
            xPortail = LARGEUR_PLATEAU;
            yPortail = HAUTEUR_PLATEAU / 2;
        }
        else if (portail == P_BAS)
        {
            xPortail = LARGEUR_PLATEAU / 2;
            yPortail = HAUTEUR_PLATEAU;
        }
        if (portail == 0)
        {
            if (derniereDirection == HAUT || derniereDirection == BAS)
            {
                if (!existePavesPomme(lesX, lesY, xPomme, yPomme, derniereDirection, lePlateau))
                {

                    if (distancePommeX == 0)
                    {
                        escivePavesPomme(lesX, lesY, xPomme, yPomme, &derniereDirection, lePlateau);
                        ordreDeplacement = 3;
                    }
                    else

                        ordreDeplacement = 2;
                }
                else
                {

                    ordreDeplacement = 3;
                }
            }
            else
            {
                if (!existePavesPomme(lesX, lesY, xPomme, yPomme, derniereDirection, lePlateau))
                {

                    ordreDeplacement = 3;
                }
                else
                {
                    if (distancePommeX == 0)
                    {
                        escivePavesPomme(lesX, lesY, xPomme, yPomme, &derniereDirection, lePlateau);
                    }
                    else if (distancePommeY == 0)
                    {
                        escivePavesPomme(lesX, lesY, xPomme, yPomme, &derniereDirection, lePlateau);
                    }
                    else
                        ordreDeplacement = 2;
                }
            }
        }
        else
        {
            if (portail == P_HAUT || portail == P_BAS)
            {
                if (existePavesPortail(lesX, lesY, xPortail, yPortail, derniereDirection, lePlateau))
                {

                    escivePavesPortail(lesX, lesY, xPortail, yPortail, &derniereDirection, lePlateau);
                    ordreDeplacement = 1;
                }
                else
                {
                    if (distancePortailX != 0)
                    {
                        ordreDeplacement = 0;
                    }
                    else
                        ordreDeplacement = 1; // Aller dans la direction Y
                }
            }
            else
            {
                //!\ FONCTION NON BUGGER NE PAS TOUCHER .
                if (existePavesPortail(lesX, lesY, xPortail, yPortail, derniereDirection, lePlateau))
                {

                    escivePavesPortail(lesX, lesY, xPortail, yPortail, &derniereDirection, lePlateau);
                    ordreDeplacement = 1; // Aller dans la direction X
                }
                else
                {
                    if (distancePortailY != 0)
                    {
                        ordreDeplacement = 1; // Aller dans la direction Y
                        gotoxy(1, HAUTEUR_PLATEAU + 7);
                        printf("   \n");
                    }

                    else
                        ordreDeplacement = 0; // Aller dans la direction X
                }
            }
        }
        // traite le déplacement vers le portail en X
        if (distancePortailX != 0 && ordreDeplacement == 0)
        {
            definirDirection(&direction, &derniereDirection, &distancePortailX, &distancePortailY, ordreDeplacement, lesX, lesY);
            nbMovement = valAbsolu(distancePortailX);
            nbDeplacement += nbMovement;
            while (i++ < nbMovement && touche != STOP && !collision && !pommeMangee)
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
            definirDirection(&direction, &derniereDirection, &distancePortailX, &distancePortailY, ordreDeplacement, lesX, lesY);
            nbMovement = valAbsolu(distancePortailY);
            nbDeplacement += nbMovement;
            while (i++ < nbMovement && touche != STOP && !collision && !pommeMangee)
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
            definirDirection(&direction, &derniereDirection, &distancePommeX, &distancePommeY, ordreDeplacement, lesX, lesY);
            nbMovement = valAbsolu(distancePommeX);
            nbDeplacement += nbMovement;
            while (i++ < nbMovement && touche != STOP && !collision && !pommeMangee)
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
            definirDirection(&direction, &derniereDirection, &distancePommeX, &distancePommeY, ordreDeplacement, lesX, lesY);
            nbMovement = valAbsolu(distancePommeY);
            nbDeplacement += nbMovement;
            while (i++ < nbMovement && touche != STOP && !collision && !pommeMangee)
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