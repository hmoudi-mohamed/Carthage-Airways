#ifndef PERSONNEL_H
#define PERSONNEL_H

/* personnel.h - Module « Personnel » (le module porte le nom de la structure).
 * Les fonctions travaillent sur un fichier texte : on vérifie le résultat
 * directement dans le fichier. AUCUN printf / scanf / affichage ici. */

#include "date.h"

#define MAX_PERSONNEL 200

typedef struct {
    char matricule[12];      /* identifiant unique, ex : PN001               */
    char nom[30];
    char prenom[30];
    char cin[9];             /* 8 chiffres                                   */
    char fonction[16];       /* Pilote / Copilote / Hôtesse / Steward        */
    char base[24];           /* base d'attache                               */
    Date embauche;           /* date d'embauche                              */
    int  heures;             /* heures de vol                                */
    char licence[16];        /* numéro de licence                            */
    Date expiration;         /* date d'expiration de la licence              */
    char tel[9];             /* 8 chiffres                                   */
    char email[48];
    char statut[16];         /* Disponible / En vol / En congé               */
} Personnel;

/* ---- fonctions de gestion du fichier -------------------------------- */
int  ajouter_personnel(char *fichier, Personnel p);               /* 1 ok / 0 matricule déjà utilisé / -2 CIN déjà utilisé / -1 erreur fichier */
int  modifier_personnel(char *fichier, char *matricule, Personnel p); /* 1 ok / 0 introuvable / -2 CIN déjà utilisé / -1 erreur fichier */
int  supprimer_personnel(char *fichier, char *matricule);         /* 1 ok / 0 introuvable / -1 erreur fichier */
Personnel chercher_personnel(char *fichier, char *matricule);     /* p.matricule vide si introuvable */
int  modifier_coordonnees(char *fichier, char *matricule, char *tel, char *email);  /* 1 ok / 0 introuvable / -1 erreur */

int  charger_personnel(char *fichier, Personnel tab[], int max);  /* nombre lu, -1 si fichier absent */
int  sauvegarder_personnel(char *fichier, Personnel tab[], int n);/* 1 ok / -1 erreur */

/* ---- recherche, tri, statistiques ----------------------------------- */
/* critere : 0 = tous les champs, 1 = matricule, 2 = nom, 3 = prénom, 4 = base
 * fonction / statut : chaîne vide = pas de filtre */
int  filtrer_personnel(Personnel tab[], int n, Personnel res[], char *texte, int critere, char *fonction, char *statut);
void trier_personnel(Personnel tab[], int n, int croissant);      /* tri sur le nom */
int  compter_fonction(Personnel tab[], int n, char *fonction);
int  compter_statut(Personnel tab[], int n, char *statut);
int  jours_restants(Date d);                                      /* jours entre aujourd'hui et d (négatif si la date est passée) */

/* ---- contrôle de saisie --------------------------------------------- */
int   valider_personnel(Personnel p);        /* 0 = valide, sinon numéro de l'erreur */
char *message_erreur_personnel(int code);

#endif
