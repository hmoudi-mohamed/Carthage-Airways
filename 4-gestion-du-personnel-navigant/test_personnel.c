/* test_personnel.c - Teste le module Personnel SANS aucun affichage.
 * Les résultats se lisent directement dans les fichiers texte :
 *   test_personnel.txt    : fichier de données après les opérations
 *   tri_personnel.txt     : fichier trié par nom
 *   rapport_personnel.txt : liste des vérifications (ECHEC ... ou "0 echec")
 * Code retour 0 = tout est correct. */
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "personnel.h"

static int total = 0, echecs = 0;
static FILE *rapport;

#define VERIF(cond) do { total++; if (!(cond)) { echecs++; fprintf(rapport, "ECHEC ligne %d : %s\n", __LINE__, #cond); } } while (0)

static Personnel faire(char *mat, char *nom, char *prenom, char *cin, char *fonction, char *base,
                       int heures, char *licence, int ja, int ma, int aa, char *tel, char *email, char *statut)
{
    Personnel x;
    memset(&x, 0, sizeof x);
    strcpy(x.matricule, mat); strcpy(x.nom, nom); strcpy(x.prenom, prenom); strcpy(x.cin, cin);
    strcpy(x.fonction, fonction); strcpy(x.base, base);
    x.embauche.jour = 15; x.embauche.mois = 3; x.embauche.annee = 2010;
    x.heures = heures; strcpy(x.licence, licence);
    x.expiration.jour = ja; x.expiration.mois = ma; x.expiration.annee = aa;
    strcpy(x.tel, tel); strcpy(x.email, email); strcpy(x.statut, statut);
    return x;
}

int main(void)
{
    char *fic = "test_personnel.txt";
    Personnel tab[MAX_PERSONNEL], res[MAX_PERSONNEL], x;
    Personnel p1 = faire("PN001", "Ben Salah", "Karim", "09123456", "Pilote", "Tunis-Carthage", 8450, "LIC-P-1001", 20, 3, 2027, "98123456", "karim@mail.tn", "Disponible");
    Personnel p2 = faire("PN002", "Gharbi", "Mohamed", "08234567", "Copilote", "Tunis-Carthage", 3120, "LIC-P-1002", 15, 12, 2026, "97234567", "mohamed@mail.tn", "Disponible");
    Personnel p3 = faire("PN003", "Trabelsi", "Sarra", "07345678", "Hôtesse", "Djerba-Zarzis", 2100, "LIC-C-2001", 30, 6, 2027, "55345678", "sarra@mail.tn", "En vol");
    int n, k, i;
    Date d;
    time_t t = time(NULL);
    struct tm *l = localtime(&t);

    rapport = fopen("rapport_personnel.txt", "w");
    if (rapport == NULL) return 2;
    remove(fic);

    /* 1. validation */
    VERIF(valider_personnel(p1) == 0);
    x = p1; strcpy(x.matricule, "P");           VERIF(valider_personnel(x) == 1);
    x = p1; strcpy(x.nom, "");                  VERIF(valider_personnel(x) == 2);
    x = p1; strcpy(x.prenom, "K4rim");          VERIF(valider_personnel(x) == 3);
    x = p1; strcpy(x.cin, "123");               VERIF(valider_personnel(x) == 4);
    x = p1; strcpy(x.fonction, "");             VERIF(valider_personnel(x) == 5);
    x = p1; strcpy(x.licence, "L");             VERIF(valider_personnel(x) == 6);
    x = p1; x.expiration.jour = 31; x.expiration.mois = 11; VERIF(valider_personnel(x) == 7);
    x = p1; x.embauche.mois = 13;               VERIF(valider_personnel(x) == 8);
    x = p1; strcpy(x.tel, "12AB");              VERIF(valider_personnel(x) == 9);
    x = p1; strcpy(x.email, "pas-un-email");    VERIF(valider_personnel(x) == 10);
    x = p1; strcpy(x.statut, "Inconnu");        VERIF(valider_personnel(x) == 11);
    x = p1; x.heures = -1;                      VERIF(valider_personnel(x) == 12);

    /* 2. ajouter (le fichier test_personnel.txt est créé) */
    VERIF(ajouter_personnel(fic, p1) == 1);
    VERIF(ajouter_personnel(fic, p2) == 1);
    VERIF(ajouter_personnel(fic, p3) == 1);
    VERIF(ajouter_personnel(fic, p1) == 0);                 /* même matricule */
    x = p3; strcpy(x.matricule, "PN009");
    VERIF(ajouter_personnel(fic, x) == -2);                 /* même CIN */
    n = charger_personnel(fic, tab, MAX_PERSONNEL);
    VERIF(n == 3);

    /* 3. chercher */
    x = chercher_personnel(fic, "PN002");
    VERIF(strcmp(x.nom, "Gharbi") == 0 && x.heures == 3120);
    x = chercher_personnel(fic, "XXXX");
    VERIF(x.matricule[0] == '\0');

    /* 4. modifier */
    x = p3; strcpy(x.statut, "En congé"); x.heures = 2200;
    VERIF(modifier_personnel(fic, "PN003", x) == 1);
    x = chercher_personnel(fic, "PN003");
    VERIF(strcmp(x.statut, "En congé") == 0 && x.heures == 2200);
    VERIF(modifier_personnel(fic, "XXXX", p1) == 0);
    x = p3; strcpy(x.cin, p1.cin);
    VERIF(modifier_personnel(fic, "PN003", x) == -2);       /* CIN d'un autre membre */
    VERIF(modifier_coordonnees(fic, "PN001", "99999999", "nouveau@mail.tn") == 1);
    x = chercher_personnel(fic, "PN001");
    VERIF(strcmp(x.tel, "99999999") == 0 && strcmp(x.email, "nouveau@mail.tn") == 0);
    VERIF(modifier_coordonnees(fic, "XXXX", "99999999", "a@b.tn") == 0);

    /* 5. filtrer */
    n = charger_personnel(fic, tab, MAX_PERSONNEL);
    k = filtrer_personnel(tab, n, res, "", 0, "", "");                VERIF(k == 3);
    k = filtrer_personnel(tab, n, res, "tunis", 0, "", "");           VERIF(k == 2);
    k = filtrer_personnel(tab, n, res, "tunis", 4, "", "");           VERIF(k == 2);
    k = filtrer_personnel(tab, n, res, "tunis", 2, "", "");           VERIF(k == 0);
    k = filtrer_personnel(tab, n, res, "", 0, "Pilote", "");          VERIF(k == 1);
    k = filtrer_personnel(tab, n, res, "", 0, "", "Disponible");      VERIF(k == 2);

    /* 6. trier, compter, jours restants */
    trier_personnel(tab, n, 1);
    for (i = 1; i < n; i++) VERIF(strcasecmp(tab[i - 1].nom, tab[i].nom) <= 0);
    VERIF(sauvegarder_personnel("tri_personnel.txt", tab, n) == 1);
    trier_personnel(tab, n, 0);
    for (i = 1; i < n; i++) VERIF(strcasecmp(tab[i - 1].nom, tab[i].nom) >= 0);
    VERIF(compter_fonction(tab, n, "Pilote") == 1);
    VERIF(compter_statut(tab, n, "En congé") == 1);
    d.jour = 1; d.mois = 1; d.annee = 2100;  VERIF(jours_restants(d) > 0);
    d.jour = 1; d.mois = 1; d.annee = 2000;  VERIF(jours_restants(d) < 0);
    d.jour = l->tm_mday; d.mois = l->tm_mon + 1; d.annee = l->tm_year + 1900;
    VERIF(jours_restants(d) == 0);

    /* 7. supprimer */
    VERIF(supprimer_personnel(fic, "PN003") == 1);
    VERIF(supprimer_personnel(fic, "PN003") == 0);
    n = charger_personnel(fic, tab, MAX_PERSONNEL);
    VERIF(n == 2);

    /* 8. fichier absent */
    VERIF(charger_personnel("fichier_inexistant.txt", tab, MAX_PERSONNEL) == -1);

    fprintf(rapport, "%d verifications, %d echec(s)\n", total, echecs);
    fclose(rapport);
    return echecs == 0 ? 0 : 1;
}
