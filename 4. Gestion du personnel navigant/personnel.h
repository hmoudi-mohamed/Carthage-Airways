#ifndef PERSONNEL_H
#define PERSONNEL_H

/* personnel.h - Module « Personnel » (nom du module = nom de la structure).
   Toutes les fonctions travaillent sur un fichier texte et ne font AUCUN affichage. */

#define MAX_PERSONNEL 500
#define NB_CHAMPS_PERSONNEL 13

typedef struct {
    char matricule[9];
    char nom[31];
    char prenom[31];
    char cin[9];
    char fonction[17];
    char base[25];
    char embauche[11];
    int  heures;
    char licence[15];
    char expiration[11];
    char tel[9];
    char email[49];
    char statut[13];
} Personnel;

int  ajouter_personnel(char *fichier, Personnel x);                  /* 1 ok | 0 identifiant déjà utilisé | -2 valeur unique déjà utilisée | -1 erreur fichier */
int  modifier_personnel(char *fichier, char *id, Personnel x);       /* 1 ok | 0 introuvable | -2 valeur unique déjà utilisée | -1 erreur fichier */
int  supprimer_personnel(char *fichier, char *id);             /* 1 ok | 0 introuvable | -1 erreur fichier */
Personnel chercher_personnel(char *fichier, char *id);               /* champ « matricule » vide si introuvable */
int  charger_personnel(char *fichier, Personnel tab[], int max);     /* nombre d'éléments lus, -1 si fichier absent */
int  sauvegarder_personnel(char *fichier, Personnel tab[], int n);   /* 1 ok | -1 erreur */
int  filtrer_personnel(Personnel tab[], int n, Personnel res[], char *texte, int critere, char *val1, char *val2);
                                  /* critere 0 = tous les champs ; val1/val2 : filtres exacts ("" = pas de filtre) */
void trier_personnel(Personnel tab[], int n, int croissant);        /* tri sur le champ « nom » */
int  compter_personnel(Personnel tab[], int n, char *valeur);       /* nombre d'éléments dont « statut » = valeur */
int  valider_personnel(Personnel x);                                 /* 0 = valide, sinon numéro du champ invalide */
char *message_erreur_personnel(int code);
int  exporter_personnel(char *fichier_export, Personnel tab[], int n);  /* fichier .csv lisible dans Excel : 1 ok | -1 erreur */
long total_personnel(Personnel tab[], int n);                       /* somme du champ « heures » (Total des heures de vol) */
long date_num_personnel(char *jj_mm_aaaa);                     /* aaaammjj, 0 si invalide */

extern const char *CRITERES_PERSONNEL[];  /* libellés des critères de recherche */
#define NB_CRITERES_PERSONNEL 5

#endif
