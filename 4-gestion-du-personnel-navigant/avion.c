/* avion.c - Module « Avion » : gestion des avions dans un fichier texte.
 * Format d'une ligne :
 *   immat;modele;constructeur;type;base;jj/mm/aaaa;capacite;reservees;etat
 * Aucun printf / scanf / affichage dans ce fichier. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "avion.h"

#define LIGNE_MAX 512

/* ------------------------------------------------------------------ */
/*  Petites fonctions internes                                         */
/* ------------------------------------------------------------------ */
static void copier(char *dest, int taille, char *source)
{
    strncpy(dest, source, taille - 1);
    dest[taille - 1] = '\0';
}

static int est_vide(char *s)
{
    for (; *s; s++)
        if (!isspace((unsigned char)*s)) return 0;
    return 1;
}

/* Découpe "a;b;c" en champs (remplace les ';' par '\0'). Retourne le nombre de champs. */
static int decouper(char *ligne, char *champ[], int max)
{
    int n = 0;
    char *p;
    champ[n++] = ligne;
    for (p = ligne; *p && n < max; p++)
        if (*p == ';') {
            *p = '\0';
            champ[n++] = p + 1;
        }
    return n;
}

/* Le séparateur du fichier est ';' : on l'interdit à l'intérieur des champs. */
static void nettoyer(char *s)
{
    for (; *s; s++)
        if (*s == ';' || *s == '\n' || *s == '\r') *s = ',';
}

static int contient(char *texte, char *motif)       /* recherche sans tenir compte des majuscules */
{
    int i, j, lt = strlen(texte), lm = strlen(motif);
    if (lm == 0) return 1;
    for (i = 0; i + lm <= lt; i++) {
        for (j = 0; j < lm; j++)
            if (tolower((unsigned char)texte[i + j]) != tolower((unsigned char)motif[j])) break;
        if (j == lm) return 1;
    }
    return 0;
}

static int date_valide(Date d)
{
    int jours[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int max;
    if (d.annee < 1900 || d.annee > 2100 || d.mois < 1 || d.mois > 12) return 0;
    max = jours[d.mois - 1];
    if (d.mois == 2 && ((d.annee % 4 == 0 && d.annee % 100 != 0) || d.annee % 400 == 0)) max = 29;
    return d.jour >= 1 && d.jour <= max;
}

/* "12/03/2015" -> Date (0/0/0 si le texte est incorrect) */
static Date texte_vers_date(char *s)
{
    Date d = {0, 0, 0};
    if (strlen(s) == 10 && s[2] == '/' && s[5] == '/') {
        d.jour = atoi(s);
        d.mois = atoi(s + 3);
        d.annee = atoi(s + 6);
    }
    return d;
}

/* Une ligne du fichier -> structure Avion (retourne 1 si la ligne est correcte) */
static int ligne_vers_avion(char *ligne, Avion *a)
{
    char *c[12];
    ligne[strcspn(ligne, "\r\n")] = '\0';
    if (decouper(ligne, c, 12) < 9) return 0;
    copier(a->immat, sizeof a->immat, c[0]);
    copier(a->modele, sizeof a->modele, c[1]);
    copier(a->constructeur, sizeof a->constructeur, c[2]);
    copier(a->type, sizeof a->type, c[3]);
    copier(a->base, sizeof a->base, c[4]);
    a->service = texte_vers_date(c[5]);
    a->capacite = atoi(c[6]);
    a->reservees = atoi(c[7]);
    copier(a->etat, sizeof a->etat, c[8]);
    return 1;
}

/* Structure Avion -> une ligne du fichier */
static void avion_vers_ligne(Avion a, char *ligne, int taille)
{
    nettoyer(a.immat); nettoyer(a.modele); nettoyer(a.constructeur);
    nettoyer(a.type); nettoyer(a.base); nettoyer(a.etat);
    snprintf(ligne, taille, "%s;%s;%s;%s;%s;%02d/%02d/%04d;%d;%d;%s\n",
             a.immat, a.modele, a.constructeur, a.type, a.base,
             a.service.jour, a.service.mois, a.service.annee,
             a.capacite, a.reservees, a.etat);
}

/* ------------------------------------------------------------------ */
/*  Lecture / écriture du fichier                                      */
/* ------------------------------------------------------------------ */
int charger_avions(char *fichier, Avion tab[], int max)
{
    FILE *f = fopen(fichier, "r");
    char ligne[LIGNE_MAX];
    int n = 0;

    if (f == NULL) return -1;
    while (n < max && fgets(ligne, sizeof ligne, f) != NULL)
        if (ligne_vers_avion(ligne, &tab[n])) n++;
    fclose(f);
    return n;
}

int sauvegarder_avions(char *fichier, Avion tab[], int n)
{
    FILE *f = fopen(fichier, "w");
    char ligne[LIGNE_MAX];
    int i;

    if (f == NULL) return -1;
    for (i = 0; i < n; i++) {
        avion_vers_ligne(tab[i], ligne, sizeof ligne);
        fputs(ligne, f);
    }
    fclose(f);
    return 1;
}

/* ------------------------------------------------------------------ */
/*  Ajouter / modifier / supprimer / chercher                          */
/* ------------------------------------------------------------------ */
int ajouter_avion(char *fichier, Avion a)
{
    Avion tab[MAX_AVIONS];
    char ligne[LIGNE_MAX];
    int n, i;
    FILE *f;

    n = charger_avions(fichier, tab, MAX_AVIONS);
    if (n < 0) n = 0;                          /* fichier pas encore créé : il sera créé */
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].immat, a.immat) == 0) return 0;     /* déjà existant */

    f = fopen(fichier, "a");                   /* "a" : ajout à la fin du fichier */
    if (f == NULL) return -1;
    avion_vers_ligne(a, ligne, sizeof ligne);
    fputs(ligne, f);
    fclose(f);
    return 1;
}

int modifier_avion(char *fichier, char *immat, Avion a)
{
    Avion tab[MAX_AVIONS];
    int n, i, trouve = 0;

    n = charger_avions(fichier, tab, MAX_AVIONS);
    if (n < 0) return -1;
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].immat, immat) == 0) {
            copier(a.immat, sizeof a.immat, immat);   /* l'immatriculation ne change pas */
            tab[i] = a;
            trouve = 1;
        }
    if (!trouve) return 0;
    return sauvegarder_avions(fichier, tab, n);
}

int supprimer_avion(char *fichier, char *immat)
{
    Avion tab[MAX_AVIONS];
    int n, i, k = 0;

    n = charger_avions(fichier, tab, MAX_AVIONS);
    if (n < 0) return -1;
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].immat, immat) != 0) tab[k++] = tab[i];   /* on garde tous les autres */
    if (k == n) return 0;                                           /* rien supprimé */
    return sauvegarder_avions(fichier, tab, k);
}

Avion chercher_avion(char *fichier, char *immat)
{
    Avion tab[MAX_AVIONS];
    Avion resultat;
    int n, i;

    memset(&resultat, 0, sizeof resultat);          /* resultat.immat = "" : introuvable */
    n = charger_avions(fichier, tab, MAX_AVIONS);
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].immat, immat) == 0) {
            resultat = tab[i];
            break;
        }
    return resultat;
}

/* ------------------------------------------------------------------ */
/*  Recherche, tri, statistiques                                       */
/* ------------------------------------------------------------------ */
int filtrer_avions(Avion tab[], int n, Avion res[], char *texte, int critere, char *type, char *etat)
{
    int i, k = 0, ok;

    for (i = 0; i < n; i++) {
        if (*type && strcmp(tab[i].type, type) != 0) continue;
        if (*etat && strcmp(tab[i].etat, etat) != 0) continue;

        ok = 0;
        if ((critere == 0 || critere == 1) && contient(tab[i].immat, texte)) ok = 1;
        if ((critere == 0 || critere == 2) && contient(tab[i].modele, texte)) ok = 1;
        if ((critere == 0 || critere == 3) && contient(tab[i].constructeur, texte)) ok = 1;
        if ((critere == 0 || critere == 4) && contient(tab[i].base, texte)) ok = 1;
        if (ok) res[k++] = tab[i];
    }
    return k;
}

static int comparer_modeles(const void *a, const void *b)
{
    return strcasecmp(((const Avion *)a)->modele, ((const Avion *)b)->modele);
}

static int comparer_modeles_inverse(const void *a, const void *b)
{
    return comparer_modeles(b, a);
}

void trier_avions(Avion tab[], int n, int croissant)
{
    if (croissant) qsort(tab, n, sizeof(Avion), comparer_modeles);
    else           qsort(tab, n, sizeof(Avion), comparer_modeles_inverse);
}

int compter_etat(Avion tab[], int n, char *etat)
{
    int i, c = 0;
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].etat, etat) == 0) c++;
    return c;
}

int places_disponibles(Avion a)
{
    return a.capacite - a.reservees;
}

/* ------------------------------------------------------------------ */
/*  Contrôle de saisie                                                 */
/* ------------------------------------------------------------------ */
int valider_avion(Avion a)
{
    int i;

    if (strlen(a.immat) < 3) return 1;
    for (i = 0; a.immat[i]; i++)
        if (!isalnum((unsigned char)a.immat[i]) && a.immat[i] != '-') return 1;
    if (est_vide(a.modele) || strchr(a.modele, ';')) return 2;
    if (est_vide(a.type)) return 3;
    if (a.capacite < 1) return 4;
    if (a.reservees < 0 || a.reservees > a.capacite) return 5;
    if (!date_valide(a.service)) return 6;
    if (strcmp(a.etat, "Opérationnel") != 0 && strcmp(a.etat, "En maintenance") != 0 &&
        strcmp(a.etat, "Hors service") != 0) return 7;
    return 0;
}

char *message_erreur_avion(int code)
{
    switch (code) {
        case 1: return "Immatriculation invalide (3 caractères minimum : lettres, chiffres ou tiret).";
        case 2: return "Le modèle est obligatoire.";
        case 3: return "Choisissez le type d'avion.";
        case 4: return "La capacité maximale doit être au moins 1.";
        case 5: return "Les places réservées doivent être comprises entre 0 et la capacité.";
        case 6: return "La date de mise en service est invalide.";
        case 7: return "Choisissez l'état de l'avion.";
    }
    return "";
}
