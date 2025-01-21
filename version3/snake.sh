#!/bin/bash
REALPATH="$(realpath "$0")"
FULLPATH="$(dirname "$REALPATH")"
OUTPUT="version3"

# compilation séparée
for file in "$FULLPATH"/*.c
do
  gcc -Wall -c "$file" -o "$FULLPATH/$(basename "${file%.c}").o"
done

# edition de lien
gcc -Wall "$FULLPATH"/*.o -o "$FULLPATH/$OUTPUT"
rm "$FULLPATH"/*.o

# lancement du programme ... si tout va bien
if [ $# -eq 1 ]
then
  if [ $1 = "--run" ]
  then
    "$FULLPATH/$OUTPUT"
  fi
fi