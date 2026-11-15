
#ifndef AVION_H_INCLUDED
#define AVION_H_INCLUDED
#include <stdio.h>

typedef struct
{
    char immatriculation[20];
    char modele[30];
    char constructeur[30];
    int type;
    char base[30];
    int jour, mois, annee;
    int capacite_max;
    int etat;
} avion;

int ajouter(char *, avion );
int modifier(char *, char *, avion );
int supprimer(char *, char * );
avion chercher(char *, char *);
#endif

