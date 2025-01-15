#include "portails.h"

int calculDistance(int position1, int position2)
{
    // calcul la différence entre deux position
    return position1 - position2;
}
void choixTrajectoire(int lesX[], int lesY[], int xPomme, int yPomme, int *portail, int *distanceObjectifeX, int *distanceObjectifeY)
{
    // choisie si il faut prendre un portail et quel portail prendre
    // et calcule renvoie la valeur des distance du portail et de la pomme une fois sortie du portail
    int choixPortail1, choixPortail2, nonPortail;
    if (xPomme < LARGEUR_PLATEAU / 2)
    {
        if (yPomme < HAUTEUR_PLATEAU / 2)
        {
            // La pomme est sur la partie en haut à gauche du plateau

            // addition le nombre de mouvemant pour allez jusqu'à la pomme pour les différent choix

            // portail en bas
            choixPortail1 = valAbsolu(calculDistance(LARGEUR_PLATEAU / 2, lesX[0])) + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU / 2)) + valAbsolu(calculDistance(HAUTEUR_PLATEAU, lesY[0])) + valAbsolu(calculDistance(yPomme, 1));

            // portail à droite
            choixPortail2 = valAbsolu(calculDistance(LARGEUR_PLATEAU, lesX[0])) + valAbsolu(calculDistance(xPomme, 1)) + valAbsolu(calculDistance(HAUTEUR_PLATEAU / 2, lesY[0])) + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU / 2));

            nonPortail = valAbsolu(calculDistance(xPomme, lesX[0])) + valAbsolu(calculDistance(yPomme, lesY[0]));

            // compare et attribue le parcure a faire en fonction des résultat
            if (choixPortail1 < nonPortail)
            {
                if (choixPortail1 < choixPortail2)
                {
                    *portail = P_BAS;
                    *distanceObjectifeX = calculDistance(LARGEUR_PLATEAU / 2, lesX[0]);
                    *distanceObjectifeY = calculDistance(HAUTEUR_PLATEAU, lesY[0]) + 1;
                }
                else
                {
                    *portail = P_DROITE;
                    *distanceObjectifeX = calculDistance(LARGEUR_PLATEAU, lesX[0]) + 1;
                    *distanceObjectifeY = calculDistance(HAUTEUR_PLATEAU / 2, lesY[0]);
                }
            }
            else if (choixPortail2 < nonPortail)
            {
                *portail = P_DROITE;
                *distanceObjectifeX = calculDistance(LARGEUR_PLATEAU, lesX[0]) + 1;
                *distanceObjectifeY = calculDistance(HAUTEUR_PLATEAU / 2, lesY[0]);
            }
            else
            {
                *portail = 0;
                *distanceObjectifeX = calculDistance(xPomme, lesX[0]);
                *distanceObjectifeY = calculDistance(yPomme, lesY[0]);
            }
        }
        else
        {
            // La pomme est sur la partie en bas à gauche du plateau

            // addition le nombre de mouvemant pour allez jusqu'à la pomme pour les différent choix

            // portail en haut
            choixPortail1 = valAbsolu(calculDistance(LARGEUR_PLATEAU / 2, lesX[0])) + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU / 2)) + valAbsolu(calculDistance(1, lesY[0])) + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU));

            // portail à droite
            choixPortail2 = valAbsolu(calculDistance(LARGEUR_PLATEAU, lesX[0])) + valAbsolu(calculDistance(xPomme, 1)) + valAbsolu(calculDistance(HAUTEUR_PLATEAU / 2, lesY[0])) + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU / 2));

            nonPortail = valAbsolu(calculDistance(xPomme, lesX[0])) + valAbsolu(calculDistance(yPomme, lesY[0]));

            // compare et attribue le parcure a faire en fonction des résultat
            if (choixPortail1 < nonPortail)
            {
                if (choixPortail1 < choixPortail2)
                {
                    *portail = P_HAUT;
                    *distanceObjectifeX = calculDistance(LARGEUR_PLATEAU / 2, lesX[0]) - 1;
                    *distanceObjectifeY = calculDistance(1, lesY[0]);
                }
                else
                {
                    *portail = P_DROITE;
                    *distanceObjectifeX = calculDistance(LARGEUR_PLATEAU, lesX[0]) + 1;
                    *distanceObjectifeY = calculDistance(HAUTEUR_PLATEAU / 2, lesY[0]);
                }
            }
            else if (choixPortail2 < nonPortail)
            {
                *portail = P_DROITE;
                *distanceObjectifeX = calculDistance(LARGEUR_PLATEAU, lesX[0]) + 1;
                *distanceObjectifeY = calculDistance(HAUTEUR_PLATEAU / 2, lesY[0]);
            }
            else
            {
                *portail = 0;
                *distanceObjectifeX = calculDistance(xPomme, lesX[0]);
                *distanceObjectifeY = calculDistance(yPomme, lesY[0]);
            }
        }
    }
    else
    {
        if (yPomme < HAUTEUR_PLATEAU / 2)
        {
            // La pomme est sur la partie en haut à droite du plateau

            // addition le nombre de mouvemant pour allez jusqu'à la pomme pour les différent choix

            // portail en bas
            choixPortail1 = valAbsolu(calculDistance(LARGEUR_PLATEAU / 2, lesX[0])) + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU / 2)) + valAbsolu(calculDistance(HAUTEUR_PLATEAU, lesY[0])) + valAbsolu(calculDistance(yPomme, 1));
            // portail à gauche
            choixPortail2 = valAbsolu(calculDistance(1, lesX[0])) + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU)) + valAbsolu(calculDistance(HAUTEUR_PLATEAU / 2, lesY[0])) + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU / 2));

            nonPortail = valAbsolu(calculDistance(xPomme, lesX[0])) + valAbsolu(calculDistance(yPomme, lesY[0]));

            // compare et attribue le parcure a faire en fonction des résultat
            if (choixPortail1 < nonPortail)
            {
                if (choixPortail1 < choixPortail2)
                {
                    *portail = P_BAS;
                    *distanceObjectifeX = calculDistance(LARGEUR_PLATEAU / 2, lesX[0]);
                    *distanceObjectifeY = calculDistance(HAUTEUR_PLATEAU, lesY[0]) + 1;
                }
                else
                {
                    *portail = P_GAUCHE;
                    *distanceObjectifeX = calculDistance(1, lesX[0]) - 1;
                    *distanceObjectifeY = calculDistance(HAUTEUR_PLATEAU / 2, lesY[0]);
                }
            }
            else if (choixPortail2 < nonPortail)
            {
                *portail = P_GAUCHE;
                *distanceObjectifeX = calculDistance(1, lesX[0]) - 1;
                *distanceObjectifeY = calculDistance(HAUTEUR_PLATEAU / 2, lesY[0]);
            }
            else
            {
                *portail = 0;
                *distanceObjectifeX = calculDistance(xPomme, lesX[0]);
                *distanceObjectifeY = calculDistance(yPomme, lesY[0]);
            }
        }
        else
        {
            // La pomme est sur la partie en haut à froite du plateau

            // addition le nombre de mouvemant pour allez jusqu'à la pomme pour les différent choix

            // portail en haut
            choixPortail1 = valAbsolu(calculDistance(LARGEUR_PLATEAU / 2, lesX[0])) + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU / 2)) + valAbsolu(calculDistance(1, lesY[0])) + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU));

            // portail à gauche
            choixPortail2 = valAbsolu(calculDistance(1, lesX[0])) + valAbsolu(calculDistance(xPomme, LARGEUR_PLATEAU)) + valAbsolu(calculDistance(HAUTEUR_PLATEAU / 2, lesY[0])) + valAbsolu(calculDistance(yPomme, HAUTEUR_PLATEAU / 2));

            nonPortail = valAbsolu(calculDistance(xPomme, lesX[0])) + valAbsolu(calculDistance(yPomme, lesY[0]));

            if (choixPortail1 < nonPortail)
            {
                if (choixPortail1 < choixPortail2)
                {
                    *portail = P_HAUT;
                    *distanceObjectifeX = calculDistance(LARGEUR_PLATEAU / 2, lesX[0]) - 1;
                    *distanceObjectifeY = calculDistance(1, lesY[0]);
                }
                else
                {
                    *portail = P_GAUCHE;
                    *distanceObjectifeX = calculDistance(1, lesX[0]) - 1;
                    *distanceObjectifeY = calculDistance(HAUTEUR_PLATEAU / 2, lesY[0]);
                }
            }
            else if (choixPortail2 < nonPortail)
            {
                *portail = P_GAUCHE;
                *distanceObjectifeX = calculDistance(1, lesX[0]) - 1;
                *distanceObjectifeY = calculDistance(HAUTEUR_PLATEAU / 2, lesY[0]);
            }
            else
            {
                *portail = 0;
                *distanceObjectifeX = calculDistance(xPomme, lesX[0]);
                *distanceObjectifeY = calculDistance(yPomme, lesY[0]);
            }
        }
    }
}