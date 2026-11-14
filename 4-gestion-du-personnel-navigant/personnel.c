/* personnel.c - Module « Personnel » : gestion du personnel navigant dans un fichier texte.
 * Format d'une ligne :
 *   matricule;nom;prenom;cin;fonction;base;jj/mm/aaaa;heures;licence;jj/mm/aaaa;tel;email;statut
 * Aucun printf / scanf / affichage dans ce fichier. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <time.h>
#include "personnel.h"

#define LIGNE_MAX 700

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

static void nettoyer(char *s)          /* ';' est le séparateur : interdit dans les champs */
{
    for (; *s; s++)
        if (*s == ';' || *s == '\n' || *s == '\r') *s = ',';
}

static int contient(char *texte, char *motif)
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

static int ligne_vers_personnel(char *ligne, Personnel *p)
{
    char *c[16];
    ligne[strcspn(ligne, "\r\n")] = '\0';
    if (decouper(ligne, c, 16) < 13) return 0;
    copier(p->matricule, sizeof p->matricule, c[0]);
    copier(p->nom, sizeof p->nom, c[1]);
    copier(p->prenom, sizeof p->prenom, c[2]);
    copier(p->cin, sizeof p->cin, c[3]);
    copier(p->fonction, sizeof p->fonction, c[4]);
    copier(p->base, sizeof p->base, c[5]);
    p->embauche = texte_vers_date(c[6]);
    p->heures = atoi(c[7]);
    copier(p->licence, sizeof p->licence, c[8]);
    p->expiration = texte_vers_date(c[9]);
    copier(p->tel, sizeof p->tel, c[10]);
    copier(p->email, sizeof p->email, c[11]);
    copier(p->statut, sizeof p->statut, c[12]);
    return 1;
}

static void personnel_vers_ligne(Personnel p, char *ligne, int taille)
{
    nettoyer(p.matricule); nettoyer(p.nom); nettoyer(p.prenom); nettoyer(p.cin);
    nettoyer(p.fonction); nettoyer(p.base); nettoyer(p.licence);
    nettoyer(p.tel); nettoyer(p.email); nettoyer(p.statut);
    snprintf(ligne, taille, "%s;%s;%s;%s;%s;%s;%02d/%02d/%04d;%d;%s;%02d/%02d/%04d;%s;%s;%s\n",
             p.matricule, p.nom, p.prenom, p.cin, p.fonction, p.base,
             p.embauche.jour, p.embauche.mois, p.embauche.annee, p.heures, p.licence,
             p.expiration.jour, p.expiration.mois, p.expiration.annee,
             p.tel, p.email, p.statut);
}

/* ------------------------------------------------------------------ */
/*  Lecture / écriture du fichier                                      */
/* ------------------------------------------------------------------ */
int charger_personnel(char *fichier, Personnel tab[], int max)
{
    FILE *f = fopen(fichier, "r");
    char ligne[LIGNE_MAX];
    int n = 0;

    if (f == NULL) return -1;
    while (n < max && fgets(ligne, sizeof ligne, f) != NULL)
        if (ligne_vers_personnel(ligne, &tab[n])) n++;
    fclose(f);
    return n;
}

int sauvegarder_personnel(char *fichier, Personnel tab[], int n)
{
    FILE *f = fopen(fichier, "w");
    char ligne[LIGNE_MAX];
    int i;

    if (f == NULL) return -1;
    for (i = 0; i < n; i++) {
        personnel_vers_ligne(tab[i], ligne, sizeof ligne);
        fputs(ligne, f);
    }
    fclose(f);
    return 1;
}

/* ------------------------------------------------------------------ */
/*  Ajouter / modifier / supprimer / chercher                          */
/* ------------------------------------------------------------------ */
int ajouter_personnel(char *fichier, Personnel p)
{
    Personnel tab[MAX_PERSONNEL];
    char ligne[LIGNE_MAX];
    int n, i;
    FILE *f;

    n = charger_personnel(fichier, tab, MAX_PERSONNEL);
    if (n < 0) n = 0;                          /* fichier pas encore créé : il sera créé */
    for (i = 0; i < n; i++) {
        if (strcmp(tab[i].matricule, p.matricule) == 0) return 0;   /* matricule déjà utilisé */
        if (strcmp(tab[i].cin, p.cin) == 0) return -2;              /* CIN déjà utilisé */
    }
    f = fopen(fichier, "a");                   /* "a" : ajout à la fin du fichier */
    if (f == NULL) return -1;
    personnel_vers_ligne(p, ligne, sizeof ligne);
    fputs(ligne, f);
    fclose(f);
    return 1;
}

int modifier_personnel(char *fichier, char *matricule, Personnel p)
{
    Personnel tab[MAX_PERSONNEL];
    int n, i, pos = -1;

    n = charger_personnel(fichier, tab, MAX_PERSONNEL);
    if (n < 0) return -1;
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].matricule, matricule) == 0) pos = i;
    if (pos < 0) return 0;
    for (i = 0; i < n; i++)
        if (i != pos && strcmp(tab[i].cin, p.cin) == 0) return -2;   /* CIN pris par un autre */
    copier(p.matricule, sizeof p.matricule, matricule);              /* le matricule ne change pas */
    tab[pos] = p;
    return sauvegarder_personnel(fichier, tab, n);
}

int supprimer_personnel(char *fichier, char *matricule)
{
    Personnel tab[MAX_PERSONNEL];
    int n, i, k = 0;

    n = charger_personnel(fichier, tab, MAX_PERSONNEL);
    if (n < 0) return -1;
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].matricule, matricule) != 0) tab[k++] = tab[i];
    if (k == n) return 0;
    return sauvegarder_personnel(fichier, tab, k);
}

Personnel chercher_personnel(char *fichier, char *matricule)
{
    Personnel tab[MAX_PERSONNEL];
    Personnel resultat;
    int n, i;

    memset(&resultat, 0, sizeof resultat);        /* resultat.matricule = "" : introuvable */
    n = charger_personnel(fichier, tab, MAX_PERSONNEL);
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].matricule, matricule) == 0) {
            resultat = tab[i];
            break;
        }
    return resultat;
}

/* Le membre du personnel ne modifie que son téléphone et son email. */
int modifier_coordonnees(char *fichier, char *matricule, char *tel, char *email)
{
    Personnel tab[MAX_PERSONNEL];
    int n, i;

    n = charger_personnel(fichier, tab, MAX_PERSONNEL);
    if (n < 0) return -1;
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].matricule, matricule) == 0) {
            copier(tab[i].tel, sizeof tab[i].tel, tel);
            copier(tab[i].email, sizeof tab[i].email, email);
            return sauvegarder_personnel(fichier, tab, n);
        }
    return 0;
}

/* ------------------------------------------------------------------ */
/*  Recherche, tri, statistiques                                       */
/* ------------------------------------------------------------------ */
int filtrer_personnel(Personnel tab[], int n, Personnel res[], char *texte, int critere, char *fonction, char *statut)
{
    int i, k = 0, ok;

    for (i = 0; i < n; i++) {
        if (*fonction && strcmp(tab[i].fonction, fonction) != 0) continue;
        if (*statut && strcmp(tab[i].statut, statut) != 0) continue;

        ok = 0;
        if ((critere == 0 || critere == 1) && contient(tab[i].matricule, texte)) ok = 1;
        if ((critere == 0 || critere == 2) && contient(tab[i].nom, texte)) ok = 1;
        if ((critere == 0 || critere == 3) && contient(tab[i].prenom, texte)) ok = 1;
        if ((critere == 0 || critere == 4) && contient(tab[i].base, texte)) ok = 1;
        if (ok) res[k++] = tab[i];
    }
    return k;
}

static int comparer_noms(const void *a, const void *b)
{
    return strcasecmp(((const Personnel *)a)->nom, ((const Personnel *)b)->nom);
}

static int comparer_noms_inverse(const void *a, const void *b)
{
    return comparer_noms(b, a);
}

void trier_personnel(Personnel tab[], int n, int croissant)
{
    if (croissant) qsort(tab, n, sizeof(Personnel), comparer_noms);
    else           qsort(tab, n, sizeof(Personnel), comparer_noms_inverse);
}

int compter_fonction(Personnel tab[], int n, char *fonction)
{
    int i, c = 0;
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].fonction, fonction) == 0) c++;
    return c;
}

int compter_statut(Personnel tab[], int n, char *statut)
{
    int i, c = 0;
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].statut, statut) == 0) c++;
    return c;
}

/* Nombre de jours entre aujourd'hui et la date d (négatif si la date est passée). */
int jours_restants(Date d)
{
    struct tm cible, aujourdhui;
    time_t maintenant = time(NULL);
    long ecart;

    aujourdhui = *localtime(&maintenant);
    aujourdhui.tm_hour = 12; aujourdhui.tm_min = 0; aujourdhui.tm_sec = 0;   /* midi : évite les soucis d'heure d'été */
    memset(&cible, 0, sizeof cible);
    cible.tm_mday = d.jour; cible.tm_mon = d.mois - 1; cible.tm_year = d.annee - 1900;
    cible.tm_hour = 12; cible.tm_isdst = -1;
    aujourdhui.tm_isdst = -1;
    ecart = (long)difftime(mktime(&cible), mktime(&aujourdhui));
    if (ecart >= 0) return (int)((ecart + 43200) / 86400);
    return -(int)((-ecart + 43200) / 86400);
}

/* ------------------------------------------------------------------ */
/*  Contrôle de saisie                                                 */
/* ------------------------------------------------------------------ */
static int chiffres_seulement(char *s, int longueur)
{
    int i;
    if ((int)strlen(s) != longueur) return 0;
    for (i = 0; i < longueur; i++)
        if (!isdigit((unsigned char)s[i])) return 0;
    return 1;
}

static int code_valide(char *s, int min)
{
    int i;
    if ((int)strlen(s) < min) return 0;
    for (i = 0; s[i]; i++)
        if (!isalnum((unsigned char)s[i]) && s[i] != '-') return 0;
    return 1;
}

static int nom_valide(char *s)                 /* lettres, espaces, tiret, apostrophe (accents acceptés) */
{
    int i;
    if (est_vide(s)) return 0;
    for (i = 0; s[i]; i++)
        if (!isalpha((unsigned char)s[i]) && (unsigned char)s[i] < 128 &&
            s[i] != ' ' && s[i] != '-' && s[i] != '\'') return 0;
    return 1;
}

static int email_valide(char *s)
{
    char *a = strchr(s, '@');
    if (a == NULL || a == s || strchr(a + 1, '@') != NULL) return 0;
    if (strchr(a + 1, '.') == NULL || a[1] == '.' || s[strlen(s) - 1] == '.' || strchr(s, ' ') != NULL) return 0;
    return 1;
}

int valider_personnel(Personnel p)
{
    if (!code_valide(p.matricule, 3)) return 1;
    if (!nom_valide(p.nom)) return 2;
    if (!nom_valide(p.prenom)) return 3;
    if (!chiffres_seulement(p.cin, 8)) return 4;
    if (est_vide(p.fonction)) return 5;
    if (!code_valide(p.licence, 3)) return 6;
    if (!date_valide(p.expiration)) return 7;
    if (!date_valide(p.embauche)) return 8;
    if (!chiffres_seulement(p.tel, 8)) return 9;
    if (!email_valide(p.email)) return 10;
    if (strcmp(p.statut, "Disponible") != 0 && strcmp(p.statut, "En vol") != 0 &&
        strcmp(p.statut, "En congé") != 0) return 11;
    if (p.heures < 0) return 12;
    return 0;
}

char *message_erreur_personnel(int code)
{
    switch (code) {
        case 1:  return "Matricule invalide (3 caractères minimum : lettres, chiffres ou tiret).";
        case 2:  return "Le nom est obligatoire (lettres uniquement).";
        case 3:  return "Le prénom est obligatoire (lettres uniquement).";
        case 4:  return "Le CIN doit contenir exactement 8 chiffres.";
        case 5:  return "Choisissez la fonction.";
        case 6:  return "Numéro de licence invalide (3 caractères minimum).";
        case 7:  return "La date d'expiration de la licence est invalide.";
        case 8:  return "La date d'embauche est invalide.";
        case 9:  return "Le téléphone doit contenir exactement 8 chiffres.";
        case 10: return "L'adresse email est invalide.";
        case 11: return "Choisissez le statut.";
        case 12: return "Les heures de vol ne peuvent pas être négatives.";
    }
    return "";
}
