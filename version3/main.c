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
    // distance avec objectife que le serpent veut atteindre
    int distanceObjectifeX, distanceObjectifeY;
    // position du portail
    int xPortail, yPortail;
    // nombre de mouvement dans une même direction
    int nbMovement;
    // nombre qui permet de l'odre de déplacement du serpent en X puit en Y ou invercement
    int ordreDeplacement = 0;
    // nombre permettant de géré les boucle while du main
    int i = 0;
    //précise sur quel axe le serpent doit ce déplacer 0 pour X et 1 pour Y
    int axes;

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
        choixTrajectoire(lesX, lesY, xPomme,  yPomme, &portail, &distanceObjectifeX, &distanceObjectifeY);

    } while (touche != STOP && !collision && !gagne);
    fin_t = clock();
    enableEcho();
    finDuJeu(nbPommes, debut_t, fin_t, nbDeplacement);
    return EXIT_SUCCESS;
}