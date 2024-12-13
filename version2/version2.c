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

//variable globale
// revoie la divition des dimention du plateau toujours supérieur ou égale à 1;
int QUOTIEN_DIMENTION = (LARGEUR_PLATEAU>HAUTEUR_PLATEAU)? LARGEUR_PLATEAU/HAUTEUR_PLATEAU : HAUTEUR_PLATEAU/LARGEUR_PLATEAU;



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
void ajouterPomme(tPlateau plateau, int *xPomme, int *yPomme,int nbPommes);
void afficher(int, int, char);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void calculDistance(int endroitX, int lesX[], int endroitY, int lesY[], int *movX, int *movY);
int valAbsolu(int valeur);
void choixDirection(int lesX[], int lesY[], int xPomme, int yPomme, int *portail,int *distancePommeX,int *distancePommeY);
void vaVersPortail(int portail, int lesX[], int lesY[], int *distancePortailX, int *distancePortailY);
void progresser(int lesX[], int lesY[], char direction, tPlateau plateau, bool *collision, bool *pomme);
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
    // stock si le serpent doit prendre un portail et lequel et est a 0 sinon;
    int portail = 0;
    //distance avec la pomme
    int distancePommeX, distancePommeY;
    //distance le portail a prendre
    int distancePortailX, distancePortailY;
    // valeur des modulo pour savoir l'ordre de deplacement
    int valX1Modulo, valX2Modulo;
    // nombre de mouvement dans une même direction
    int nbMovement;
    // variable servant a comter le nombre de fois que le serpent va progresser
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
    touche = DROITE;

    // boucle de jeu. Arret si touche STOP, si collision avec une bordure ou
    // si toutes les pommes sont mangées
    debut_t = clock();
    do
    {   
        choixDirection(lesX, lesY, xPomme, yPomme, &portail, &distancePommeX, &distancePommeY);
        if(portail > 0)
        {
            //calcule la direction pour allez au portail
            vaVersPortail(portail, lesX, lesY, &distancePortailX, &distancePortailY);
            //fait le chemin jusqu'au portail en X
            if ((i % 4 == valX1Modulo || i % 4 == valX2Modulo) && distancePortailX != 0)
            {
                direction = (distancePortailX < 0) ? GAUCHE : DROITE;
                nbMovement = valAbsolu(distancePortailX);
                deplacement += nbMovement;
                for (int j = 0; j < nbMovement && touche != STOP; j++)
                {
                    progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee);
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
            }
            //fait le chemin jusqu'au portail en Y
            else if (distancePortailY != 0)
            {
                direction = (distancePortailY < 0) ? HAUT : BAS;
                nbMovement = valAbsolu(distancePortailY);
                deplacement += nbMovement;
                for (int j = 0; j < nbMovement && touche != STOP ; j++)
                {
                    progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee);
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
            }
            //verifie si le portail est atteint
            if((lesX[0] == LARGEUR_PLATEAU/2 && lesY[0] == 1) ||
                (lesX[0] == 1 && lesY[0] == HAUTEUR_PLATEAU/2) ||
                (lesX[0] == LARGEUR_PLATEAU && lesY[0] == HAUTEUR_PLATEAU/2) ||
                (lesX[0] == LARGEUR_PLATEAU/2 && lesY[0] == HAUTEUR_PLATEAU))
                {portail = 0;}

        }
        else
        {
            
            if ((i % 4 == valX1Modulo || i % 4 == valX2Modulo) && distancePommeX != 0)
            {
                direction = (distancePommeX < 0) ? GAUCHE : DROITE;
                nbMovement = valAbsolu(distancePommeX);
                deplacement += nbMovement;
                for (int j = 0; j < nbMovement && touche != STOP; j++)
                {
                    progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee);
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
            }
            else if (distancePommeY != 0)
            {
                direction = (distancePommeY < 0) ? HAUT : BAS;
                nbMovement = valAbsolu(distancePommeY);
                deplacement += nbMovement;
                for (int j = 0; j < nbMovement && touche != STOP ; j++)
                {
                    progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee);
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
                // inverce l'ordre des deplacement quand necesaire
                if ((distancePommeX == 0) ^ (distancePommeY == 0))
                {
                    valX1Modulo = (valX1Modulo + 1) % 4;
                    valX2Modulo = (valX2Modulo + 1) % 4;
                }
            }
            i++;
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
            plateau[i][j] = ((i == 1 && j != HAUTEUR_PLATEAU/2) ||
                            (j == 1 && i != LARGEUR_PLATEAU/2) ||
                            (i == LARGEUR_PLATEAU && j != HAUTEUR_PLATEAU/2) ||
                            (j == HAUTEUR_PLATEAU && i != LARGEUR_PLATEAU/2)) ?  BORDURE : VIDE;
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

void ajouterPomme(tPlateau plateau, int *xPomme, int *yPomme,int nbPommes)
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

void calculDistance(int endroitX, int lesX[], int endroitY, int lesY[], int *movX, int *movY)
{
    *movX = endroitX - lesX[0];
    *movY = endroitY - lesY[0];
}

int valAbsolu(int valeur)
{
    return (valeur < 0) ? -valeur : valeur;
}
void choixDirection(int lesX[], int lesY[], int xPomme, int yPomme, int *portail,int *distancePommeX,int *distancePommeY)
{
    // on decide quel portail prendre celon la position de la pomme et du serpent

    // ecart divition les dimention du plateau toujours >= a 1;
    int movX,movY;
    portail = 0;
    calculDistance(xPomme,lesX,yPomme,lesY,&*distancePommeX, &*distancePommeY);
    movX = valAbsolu(*distancePommeX);
    movY = valAbsolu(*distancePommeY);
    if(movX >= LARGEUR_PLATEAU/2)
    {
        if(LARGEUR_PLATEAU>HAUTEUR_PLATEAU)
        {
            if(valAbsolu(movX)>= movY*QUOTIEN_DIMENTION)
            {
                if(*distancePommeX > 0)
                {
                    *portail = P_DROITE;
                }
                else
                {
                    *portail = P_GAUCHE;
                }
            }
            else
            {
                if(*distancePommeY > 0)
                {
                    *portail = P_BAS;
                }
                else
                {
                    *portail = P_HAUT;
                }
            }
        }
        else
        {
            if(movX*QUOTIEN_DIMENTION >= movY)
            {
                if(*distancePommeX > 0)
                {
                    *portail = P_DROITE;
                }
                else
                {
                    *portail = P_GAUCHE;
                }
            }
            else
            {
                if(*distancePommeY > 0)
                {
                    *portail = P_BAS;
                }
                else
                {
                    *portail = P_HAUT;
                }
            }
        }
    }
    else if(movY >= HAUTEUR_PLATEAU)
    {
        if(*distancePommeY > 0)
        {
            *portail = P_BAS;
        }
        else
        {
            *portail = P_HAUT;
        }
    }
    if(*portail == P_BAS || *portail == P_HAUT )
    {
        if(*distancePommeY > 0)
        {
            *distancePommeY = valAbsolu(*distancePommeY-(HAUTEUR_PLATEAU+1));
        }
        else
        {
            *distancePommeY = -valAbsolu(*distancePommeY-(HAUTEUR_PLATEAU+1));
        }
    }
    else if(*portail == P_GAUCHE || *portail == P_DROITE )
    {
        if(*distancePommeX > 0)
        {
            *distancePommeX = valAbsolu(*distancePommeX-(HAUTEUR_PLATEAU+1));
        }
        else
        {
            *distancePommeX = -valAbsolu(*distancePommeX-(HAUTEUR_PLATEAU+1));
        }
    }
        
}
void vaVersPortail(int portail, int lesX[], int lesY[], int *distancePortailX, int *distancePortailY)
{
    if(portail == P_HAUT)
    {
        *distancePortailX = (LARGEUR_PLATEAU/2) - lesX[0];
        *distancePortailY = 1 - lesY[0];
    }
    else if(portail == P_GAUCHE)
    {
        *distancePortailX = 1 - lesX[0];
        *distancePortailY = (HAUTEUR_PLATEAU/2) - lesY[0];
    }
    else if(portail == P_DROITE)
    {
        *distancePortailX = LARGEUR_PLATEAU - lesX[0];
        *distancePortailY = (HAUTEUR_PLATEAU/2) - lesY[0];
    }
    else
    {
        *distancePortailX = (LARGEUR_PLATEAU/2) - lesX[0];
        *distancePortailY = HAUTEUR_PLATEAU - lesY[0];
    }
    
}
void progresser(int lesX[], int lesY[], char direction, tPlateau plateau, bool *collision, bool *pomme)
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
    // faire progresser la tete dans la nouvelle direction
    switch (direction)
    {
    case HAUT:
        lesY[0] = lesY[0] - 1;
        break;
    case BAS:
        lesY[0] = lesY[0] + 1;
        break;
    case DROITE:
        lesX[0] = lesX[0] + 1;
        break;
    case GAUCHE:
        lesX[0] = lesX[0] - 1;
        break;
    }
    *pomme = false;
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
    // détection d'une collision lui même
    else
    {
        for (int i = 1; i < TAILLE; i++)
        {
            if(lesX[0] == lesX[i] && lesY[0] == lesY[i]) *collision = true;
        }
    }
    dessinerSerpent(lesX, lesY);
}

void finDuJeu(int numeroPomme, time_t debut_t, time_t fin_t)
{
    /* @brief Fin du programme , message de fin et réactivation de l'écriture dans la console*/
    enableEcho();
    gotoxy(1, HAUTEUR_PLATEAU + 1);
    double total = ((double)(fin_t - debut_t)) / CLOCKS_PER_SEC;

    printf("La partie est terminée !\n");
    printf("Votre score est de ; %d\n", numeroPomme);
    printf("Le temps total à été de %.3f secondes\n", total);
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