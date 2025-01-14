#ifndef CONST_H
#define CONST_H

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
#define ATTENTE 100000
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
// GESTION DES PAVES
#define NB_PAVES 6
#define TAILLE_PAVES 5
#define MAX_PAVES 100
#define PAVE 'P'

#endif // CONST_H