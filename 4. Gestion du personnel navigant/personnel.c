/* personnel.c - Module « Personnel » : gestion sur fichier texte. Aucun printf / scanf, aucun affichage. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "personnel.h"

#define LIGNE_MAX 1024

const char *CRITERES_PERSONNEL[] = { "Tous les champs", "Matricule", "Nom", "Prénom", "Base d'attache" };

static void copier(char *dst, size_t n, const char *src)
{
    strncpy(dst, src ? src : "", n - 1);
    dst[n - 1] = '\0';
}

static int vide(const char *s)
{
    for (; *s; s++) if (!isspace((unsigned char)*s)) return 0;
    return 1;
}

static int decouper(char *ligne, char *champ[], int max)
{
    int n = 0;
    char *p;
    champ[n++] = ligne;
    for (p = ligne; *p && n < max; p++)
        if (*p == ';') { *p = '\0'; champ[n++] = p + 1; }
    return n;
}

static void nettoyer(char *s)               /* « ; » est le séparateur du fichier */
{
    for (; *s; s++) if (*s == ';' || *s == '\n' || *s == '\r') *s = ',';
}

static int contient(const char *meule, const char *aiguille)
{
    size_t i, j, la = strlen(aiguille), lm = strlen(meule);
    if (la == 0) return 1;
    for (i = 0; i + la <= lm; i++) {
        for (j = 0; j < la; j++)
            if (tolower((unsigned char)meule[i + j]) != tolower((unsigned char)aiguille[j])) break;
        if (j == la) return 1;
    }
    return 0;
}

static int chiffres(const char *s, int len)
{
    int i;
    if ((int)strlen(s) != len) return 0;
    for (i = 0; i < len; i++) if (!isdigit((unsigned char)s[i])) return 0;
    return 1;
}

static int lettres(const char *s)
{
    if (vide(s)) return 0;
    for (; *s; s++)
        if (!(isalpha((unsigned char)*s) || (unsigned char)*s >= 128 || *s == ' ' || *s == '-' || *s == '\'')) return 0;
    return 1;
}

static int code_ok(const char *s, int min)
{
    if ((int)strlen(s) < min) return 0;
    for (; *s; s++) if (!(isalnum((unsigned char)*s) || *s == '-')) return 0;
    return 1;
}

static int courriel(const char *s)
{
    const char *a = strchr(s, '@');
    if (!a || a == s || strchr(a + 1, '@')) return 0;
    if (!strchr(a + 1, '.') || a[1] == '.' || s[strlen(s) - 1] == '.' || strchr(s, ' ')) return 0;
    return 1;
}

long date_num_personnel(char *s)
{
    int i, j, m, a, max;
    static const int jm[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (strlen(s) != 10 || s[2] != '/' || s[5] != '/') return 0;
    for (i = 0; i < 10; i++) if (i != 2 && i != 5 && !isdigit((unsigned char)s[i])) return 0;
    j = atoi(s); m = atoi(s + 3); a = atoi(s + 6);
    if (a < 1900 || a > 2100 || m < 1 || m > 12) return 0;
    max = jm[m - 1];
    if (m == 2 && ((a % 4 == 0 && a % 100 != 0) || a % 400 == 0)) max = 29;
    if (j < 1 || j > max) return 0;
    return (long)a * 10000 + m * 100 + j;
}

#define date_num date_num_personnel

static void nettoyer_personnel(Personnel *x)
{
    nettoyer(x->matricule);
    nettoyer(x->nom);
    nettoyer(x->prenom);
    nettoyer(x->cin);
    nettoyer(x->fonction);
    nettoyer(x->base);
    nettoyer(x->embauche);
    nettoyer(x->licence);
    nettoyer(x->expiration);
    nettoyer(x->tel);
    nettoyer(x->email);
    nettoyer(x->statut);
}

static int ligne_vers_personnel(char *ligne, Personnel *x)
{
    char *c[15];
    ligne[strcspn(ligne, "\r\n")] = '\0';
    if (decouper(ligne, c, 15) < 13) return 0;
    copier(x->matricule, sizeof x->matricule, c[0]);
    copier(x->nom, sizeof x->nom, c[1]);
    copier(x->prenom, sizeof x->prenom, c[2]);
    copier(x->cin, sizeof x->cin, c[3]);
    copier(x->fonction, sizeof x->fonction, c[4]);
    copier(x->base, sizeof x->base, c[5]);
    copier(x->embauche, sizeof x->embauche, c[6]);
    x->heures = atoi(c[7]);
    copier(x->licence, sizeof x->licence, c[8]);
    copier(x->expiration, sizeof x->expiration, c[9]);
    copier(x->tel, sizeof x->tel, c[10]);
    copier(x->email, sizeof x->email, c[11]);
    copier(x->statut, sizeof x->statut, c[12]);
    return 1;
}

static void personnel_vers_ligne(Personnel x, char *buf, size_t sz)
{
    nettoyer_personnel(&x);
    snprintf(buf, sz, "%s;%s;%s;%s;%s;%s;%s;%d;%s;%s;%s;%s;%s\n", x.matricule, x.nom, x.prenom, x.cin, x.fonction, x.base, x.embauche, x.heures, x.licence, x.expiration, x.tel, x.email, x.statut);
}

int charger_personnel(char *fichier, Personnel tab[], int max)
{
    FILE *f = fopen(fichier, "r");
    char ligne[LIGNE_MAX];
    int n = 0;
    if (!f) return -1;
    while (n < max && fgets(ligne, sizeof ligne, f))
        if (ligne_vers_personnel(ligne, &tab[n])) n++;
    fclose(f);
    return n;
}

int sauvegarder_personnel(char *fichier, Personnel tab[], int n)
{
    FILE *f = fopen(fichier, "w");
    char ligne[LIGNE_MAX];
    int i;
    if (!f) return -1;
    for (i = 0; i < n; i++) {
        personnel_vers_ligne(tab[i], ligne, sizeof ligne);
        fputs(ligne, f);
    }
    fclose(f);
    return 1;
}

int ajouter_personnel(char *fichier, Personnel x)
{
    Personnel *tab = malloc(sizeof(Personnel) * MAX_PERSONNEL);
    char ligne[LIGNE_MAX];
    int n, i;
    FILE *f;
    if (!tab) return -1;
    n = charger_personnel(fichier, tab, MAX_PERSONNEL);
    if (n < 0) n = 0;                               /* fichier encore inexistant : on le créera */
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].matricule, x.matricule) == 0) { free(tab); return 0; }
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].cin, x.cin) == 0 ) { free(tab); return -2; }
    free(tab);
    f = fopen(fichier, "a");
    if (!f) return -1;
    personnel_vers_ligne(x, ligne, sizeof ligne);
    fputs(ligne, f);
    fclose(f);
    return 1;
}

int modifier_personnel(char *fichier, char *id, Personnel x)
{
    Personnel *tab = malloc(sizeof(Personnel) * MAX_PERSONNEL);
    int n, i, pos = -1, r;
    if (!tab) return -1;
    n = charger_personnel(fichier, tab, MAX_PERSONNEL);
    if (n < 0) { free(tab); return -1; }
    for (i = 0; i < n; i++) if (strcmp(tab[i].matricule, id) == 0) pos = i;
    if (pos < 0) { free(tab); return 0; }
    for (i = 0; i < n; i++)
        if (i != pos && strcmp(tab[i].cin, x.cin) == 0) { free(tab); return -2; }
    copier(x.matricule, sizeof x.matricule, id);                  /* l'identifiant ne change pas */
    tab[pos] = x;
    r = sauvegarder_personnel(fichier, tab, n);
    free(tab);
    return r;
}

int supprimer_personnel(char *fichier, char *id)
{
    Personnel *tab = malloc(sizeof(Personnel) * MAX_PERSONNEL);
    int n, i, k = 0, r;
    if (!tab) return -1;
    n = charger_personnel(fichier, tab, MAX_PERSONNEL);
    if (n < 0) { free(tab); return -1; }
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].matricule, id) != 0) tab[k++] = tab[i];
    if (k == n) { free(tab); return 0; }
    r = sauvegarder_personnel(fichier, tab, k);
    free(tab);
    return r;
}

Personnel chercher_personnel(char *fichier, char *id)
{
    Personnel *tab = malloc(sizeof(Personnel) * MAX_PERSONNEL);
    Personnel r;
    int n, i;
    memset(&r, 0, sizeof r);
    if (!tab) return r;
    n = charger_personnel(fichier, tab, MAX_PERSONNEL);
    for (i = 0; i < n; i++)
        if (strcmp(tab[i].matricule, id) == 0) { r = tab[i]; break; }
    free(tab);
    return r;
}

int filtrer_personnel(Personnel tab[], int n, Personnel res[], char *texte, int critere, char *val1, char *val2)
{
    int i, k = 0;
    for (i = 0; i < n; i++) {
        Personnel *t = &tab[i];
        int ok = 0;
        if (*val1 && strcmp(t->fonction, val1) != 0) continue;
        if (*val2 && strcmp(t->statut, val2) != 0) continue;
        if ((critere == 0 || critere == 1) && contient(t->matricule, texte)) ok = 1;
        if ((critere == 0 || critere == 2) && contient(t->nom, texte)) ok = 1;
        if ((critere == 0 || critere == 3) && contient(t->prenom, texte)) ok = 1;
        if ((critere == 0 || critere == 4) && contient(t->base, texte)) ok = 1;
        if (ok) res[k++] = *t;
    }
    return k;
}

static int cmp_asc(const void *a, const void *b)
{
    return strcasecmp(((const Personnel *)a)->nom, ((const Personnel *)b)->nom);
}
static int cmp_desc(const void *a, const void *b) { return cmp_asc(b, a); }

void trier_personnel(Personnel tab[], int n, int croissant)
{
    qsort(tab, n, sizeof(Personnel), croissant ? cmp_asc : cmp_desc);
}

int compter_personnel(Personnel tab[], int n, char *valeur)
{
    int i, c = 0;
    for (i = 0; i < n; i++) if (strcmp(tab[i].statut, valeur) == 0) c++;
    return c;
}

int exporter_personnel(char *fichier_export, Personnel tab[], int n)
{
    FILE *f = fopen(fichier_export, "w");
    char ligne[LIGNE_MAX];
    int i;
    if (!f) return -1;
    fputs("Matricule;Nom;Prénom;CIN;Fonction;Base d'attache;Date d'embauche;Heures de vol;Numéro de licence;Expiration licence;Téléphone;Email;Statut\n", f);
    for (i = 0; i < n; i++) {
        personnel_vers_ligne(tab[i], ligne, sizeof ligne);
        fputs(ligne, f);
    }
    fclose(f);
    return 1;
}

long total_personnel(Personnel tab[], int n)
{
    long t = 0;
    int i;
    for (i = 0; i < n; i++) t += tab[i].heures;
    return t;
}

int valider_personnel(Personnel v)
{
    if (!code_ok(v.matricule, 3)) return 1;
    if (!lettres(v.nom)) return 2;
    if (!lettres(v.prenom)) return 3;
    if (!chiffres(v.cin, 8)) return 4;
    if (vide(v.fonction)) return 5;
    if (vide(v.base)) return 6;
    if (date_num(v.embauche) == 0) return 7;
    if (v.heures < 0 || v.heures > 30000) return 8;
    if (!code_ok(v.licence, 4)) return 9;
    if (date_num(v.expiration) == 0) return 10;
    if (!chiffres(v.tel, 8)) return 11;
    if (!courriel(v.email)) return 12;
    if (vide(v.statut)) return 13;
    if (date_num(v.expiration) < date_num(v.embauche)) return 14;
    return 0;
}

char *message_erreur_personnel(int code)
{
    static char *msg[] = {
        "Le champ « Matricule » est obligatoire ou invalide.",
        "Le champ « Nom » est obligatoire ou invalide.",
        "Le champ « Prénom » est obligatoire ou invalide.",
        "Le champ « CIN » est obligatoire ou invalide.",
        "Le champ « Fonction » est obligatoire ou invalide.",
        "Le champ « Base d'attache » est obligatoire ou invalide.",
        "Le champ « Date d'embauche » est obligatoire ou invalide.",
        "Le champ « Heures de vol » est obligatoire ou invalide.",
        "Le champ « Numéro de licence » est obligatoire ou invalide.",
        "Le champ « Expiration licence » est obligatoire ou invalide.",
        "Le champ « Téléphone » est obligatoire ou invalide.",
        "Le champ « Email » est obligatoire ou invalide.",
        "Le champ « Statut » est obligatoire ou invalide.",
        "La licence ne peut pas expirer avant la date d'embauche."
    };
    if (code < 1 || code > 14) return "";
    return msg[code - 1];
}
