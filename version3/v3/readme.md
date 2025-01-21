# Version 3 | SAÉ 1.02 - Comparaison d'approches algorithmiques

### La version3 de cette SAE doit contenir l'ajout des pavés

---



##### Coordonnées des pavés :

X : { 3,74,3,74,38,38}

Y : { 3,3,34,34,21,15}

##### Modification des coordonnées des pommes :

X : {75,75,78,2,8,78,74,2,72,5}

Y : {8,39,2,2,5,39,33,38,35,2}

---



L'éxecution forctionne grâce au script.

###### *Ce situer dans le dossier version3 :*

```bash
./snake.sh
```

SI le script de fonctionne pas ; le voici : 


```bash
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
```
