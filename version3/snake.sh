#!/bin/bash
clear
rm *.o 
rm *.out # supprimer les fichiers .o
# supprimer les fichiers .out
# compilation séparée
gcc -Wall -c main.c
gcc -Wall -c Utilitaires.c
gcc -Wall -c globales.c
gcc -Wall -c generation.c
gcc -Wall -c jeu.c
gcc -Wall -c portails.c
gcc -Wall -c direction.c
gcc -Wall -c mouvement.c
# edition de lien
gcc -Wall main.o globales.o Utilitaires.o generation.o jeu.o portails.o direction.o mouvement.o -o snake.out
# lancement du programme … si tout va bien
./snake.out