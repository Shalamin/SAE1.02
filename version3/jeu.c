#include "jeu.h"
#include "Utilitaires.h"

void dessinerSerpent(int lesX[], int lesY[])
{
    /* @brief Dessine le serpent sur le plateau de jeu */
    for (int i = 0; i < TAILLE; i++)
    {
        afficher(lesX[i], lesY[i], (i == 0) ? TETE : CORPS);
    }
}

void ajouterPomme(tPlateau plateau, int *xPomme, int *yPomme, int numeroPommes)
{
    *xPomme = lesPommesX[numeroPommes];
    *yPomme = lesPommesY[numeroPommes];

    plateau[*xPomme][*yPomme] = POMME;
    afficher(*xPomme, *yPomme, POMME);

    gotoxy(0, HAUTEUR_PLATEAU + 4);
    printf("Pomme en %d %d\n", *xPomme, *yPomme);
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