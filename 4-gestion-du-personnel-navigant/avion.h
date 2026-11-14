#ifndef AVION_H
#define AVION_H

/* avion.h - Module « Avion » (le module porte le nom de la structure).
 * Les fonctions travaillent sur un fichier texte : on vérifie le résultat
 * directement dans le fichier. AUCUN printf / scanf / affichage ici. */

#define MAX_AVIONS 200

#include "date.h"

typedef struct {
    char immat[12];          /* identifiant unique, ex : TS-IMA          */
    char modele[30];         /* ex : A320-200                            */
    char constructeur[20];   /* ex : Airbus                              */
    char type[20];           /* Court-courrier / Moyen-courrier / ...    */
    char base[24];           /* base d'attache                           */
    Date service;            /* date de mise en service                  */
    int  capacite;           /* capacité maximale (places)               */
    int  reservees;          /* places réservées                         */
    char etat[20];           /* Opérationnel / En maintenance / Hors service */
} Avion;

/* ---- fonctions de gestion du fichier -------------------------------- */
int   ajouter_avion(char *fichier, Avion a);                 /* 1 = ok, 0 = immatriculation déjà utilisée, -1 = erreur fichier */
int   modifier_avion(char *fichier, char *immat, Avion a);   /* 1 = ok, 0 = introuvable, -1 = erreur fichier */
int   supprimer_avion(char *fichier, char *immat);           /* 1 = ok, 0 = introuvable, -1 = erreur fichier */
Avion chercher_avion(char *fichier, char *immat);            /* a.immat vide si l'avion est introuvable */

int   charger_avions(char *fichier, Avion tab[], int max);   /* nombre d'avions lus, -1 si fichier absent */
int   sauvegarder_avions(char *fichier, Avion tab[], int n); /* 1 = ok, -1 = erreur */

/* ---- recherche, tri, statistiques ----------------------------------- */
/* critere : 0 = tous les champs, 1 = immatriculation, 2 = modèle, 3 = constructeur, 4 = base
 * type / etat : chaîne vide = pas de filtre */
int   filtrer_avions(Avion tab[], int n, Avion res[], char *texte, int critere, char *type, char *etat);
void  trier_avions(Avion tab[], int n, int croissant);       /* tri sur le modèle (A->Z ou Z->A) */
int   compter_etat(Avion tab[], int n, char *etat);          /* nombre d'avions dans cet état */
int   places_disponibles(Avion a);                           /* capacité - places réservées */

/* ---- contrôle de saisie --------------------------------------------- */
int   valider_avion(Avion a);            /* 0 = valide, sinon numéro de l'erreur (voir message_erreur_avion) */
char *message_erreur_avion(int code);

#endif
