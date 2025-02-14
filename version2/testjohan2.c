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
#define HAUTEUR_PLATEAU 40 //
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
#define FIN_DEPLACEMENT 0
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
void ajouterPomme(tPlateau plateau, int *xPomme, int *yPomme, int numeroPomme);
void afficher(int, int, char);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void demiTour(int lesX[], int lesY[], char directionDT);
void definirDirection(char *direction, char *lastDirection, int movX, int movY, int lesX[], int lesY[]);
void calculMouvement(int xPomme, int lesX[], int yPomme, int lesY[], int *movX, int *movY);
int valAbsolu(int valeur);
void gestionCollisions(int lesX[], int lesY[], tPlateau plateau, bool *collision, bool *pomme);
void progresser(int lesX[], int lesY[], char direction);
void finDuJeu(int numeroPomme, time_t debut_t, time_t fin_t, int deplacement);
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
    int xPomme = lesPommesX[0];
    int yPomme = lesPommesY[0];
    int deplacement = 0;
    int movX;
    int movY;
    // valeur des modulo pour savoir l'ordre de deplacement
    int valX1Modulo, valX2Modulo;
    // nombre de mouvement dans une même direction
    int nbMovement;
    // variable servant a comter le nombre de fois que le serpent va progresser
    int i = 0;
    clock_t debut_t, fin_t;

    // compteur de pommes mangées
    int nbPommes = 0;
    valX1Modulo = 0;
    valX2Modulo = 3;

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
        calculMouvement(xPomme, lesX, yPomme, lesY, &movX, &movY);

        definirDirection(&direction, &lastDirection, movX, movY, lesX, lesY);

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
            if ((movX == 0) ^ (movY == 0))
            {
                valX1Modulo = (valX1Modulo + 1) % 4;
                valX2Modulo = (valX2Modulo + 1) % 4;
            }
        }
    } while (touche != STOP && !collision && !gagne);
    fin_t = clock();
    enableEcho();

    finDuJeu(nbPommes, debut_t, fin_t, deplacement);
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
            // génération du plateau.
            plateau[i][j] = (i == 1 || i == LARGEUR_PLATEAU ||
                             j == 1 || j == HAUTEUR_PLATEAU)
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

void ajouterPomme(tPlateau plateau, int *xPomme, int *yPomme, int numeroPomme)
{
    // génère aléatoirement la position d'une pomme,
    // vérifie que ça correspond à une case vide
    // du plateau puis l'ajoute au plateau et l'affiche
    *xPomme = lesPommesX[numeroPomme];
    *yPomme = lesPommesY[numeroPomme];
    plateau[*xPomme][*yPomme] = POMME;
    afficher(*xPomme, *yPomme, POMME);
}

void afficher(int x, int y, char car)
{
    gotoxy(x, y);
    fflush(stdout);
    printf("%c", car);
}

void effacer(int x, int y)
{
    gotoxy(x, y);
    printf(" ");
}

void demiTour(int lesX[], int lesY[], char directionDT)
{
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
void dessinerSerpent(int lesX[], int lesY[])
{
    // affiche les anneaux puis la tête
    for (int i = 1; i < TAILLE; i++)
    {
        afficher(lesX[i], lesY[i], CORPS);
    }
    afficher(lesX[0], lesY[0], TETE);
}
void calculMouvement(int xPomme, int lesX[], int yPomme, int lesY[], int *movX, int *movY)
{
    *movX = xPomme - lesX[0];
    *movY = yPomme - lesY[0];
}
int valAbsolu(int valeur)
{
    return (valeur < 0) ? -valeur : valeur;
}
void progresser(int lesX[], int lesY[], char direction)
{
    // efface le dernier élément avant d'actualiser la position de tous les
    // élémentds du serpent avant de le  redessiner et détecte une
    // collision avec une pomme ou avec une bordure
    effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);
    // faire progresser la tete dans la nouvelle direction
    for (int j = TAILLE - 1; j > 0; j--)
    {
        lesX[j] = lesX[j - 1];
        lesY[j] = lesY[j - 1];
    }

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
    usleep(ATTENTE);
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
}
void finDuJeu(int numeroPomme, time_t debut_t, time_t fin_t, int deplacement)
{
    /* @brief Fin du programme , message de fin et réactivation de l'écriture dans la console*/
    enableEcho();
    gotoxy(1, HAUTEUR_PLATEAU + 1);
    double total = ((double)(fin_t - debut_t)) / CLOCKS_PER_SEC;

    printf("La partie est terminée !\n");
    printf("Votre score est de ; %d\n", numeroPomme);
    printf("Vous avez effectué %d déplacement.\n", deplacement);
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