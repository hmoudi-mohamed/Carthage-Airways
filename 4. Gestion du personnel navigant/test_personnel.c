/* test_personnel.c - Teste le module Personnel SANS aucun affichage : on lit les résultats dans les fichiers .txt */
#include <stdio.h>
#include <string.h>
#include "personnel.h"

static int echecs = 0, total = 0;
#define VERIF(cond) do { total++; if (!(cond)) { echecs++; if (rapport) fprintf(rapport, "ECHEC ligne %d : %s\n", __LINE__, #cond); } } while (0)
static FILE *rapport;

static Personnel exemple(int i)
{
    static const Personnel ex[] = {
        { "PN001", "Ben Salah", "Karim", "09112233", "Pilote", "Tunis-Carthage", "15/03/2008", 9800, "LIC-P-0001", "30/06/2027", "98112233", "karim.bensalah@carthage.tn", "Disponible" },
        { "PN002", "Mansour", "Yassine", "08223344", "Copilote", "Tunis-Carthage", "02/09/2014", 5200, "LIC-C-0002", "15/03/2027", "97223344", "yassine.mansour@carthage.tn", "En vol" },
        { "PN003", "Karoui", "Amira", "07334455", "Hôtesse", "Djerba-Zarzis", "11/05/2016", 4100, "LIC-H-0003", "20/12/2026", "55334455", "amira.karoui@carthage.tn", "Disponible" },
        { "PN004", "Chaabane", "Walid", "11445566", "Steward", "Monastir", "23/01/2019", 2600, "LIC-S-0004", "01/02/2028", "22445566", "walid.chaabane@carthage.tn", "En congé" },
        { "PN005", "Zouari", "Sonia", "06556677", "Hôtesse", "Tunis-Carthage", "07/07/2012", 6300, "LIC-H-0005", "10/10/2027", "50556677", "sonia.zouari@carthage.tn", "Disponible" },
    };
    return ex[i];
}

int main(void)
{
    Personnel tab[MAX_PERSONNEL], res[MAX_PERSONNEL], x;
    char *fic = "test_personnel.txt";
    int i, n, k;
    rapport = fopen("rapport_personnel.txt", "w");

    remove(fic);
    /* 1. validation des exemples */
    for (i = 0; i < 5; i++) VERIF(valider_personnel(exemple(i)) == 0);
    /* 2. ajout */
    for (i = 0; i < 5; i++) VERIF(ajouter_personnel(fic, exemple(i)) == 1);
    VERIF(ajouter_personnel(fic, exemple(0)) == 0);                     /* identifiant déjà utilisé */
    n = charger_personnel(fic, tab, MAX_PERSONNEL);
    VERIF(n == 5);
    /* 3. recherche */
    x = chercher_personnel(fic, exemple(2).matricule);
    VERIF(strcmp(x.matricule, exemple(2).matricule) == 0);
    x = chercher_personnel(fic, "ZZZZ");
    VERIF(x.matricule[0] == '\0');
    /* 4. modification */
    x = exemple(1);
    strcpy(x.fonction, "Steward");
    VERIF(modifier_personnel(fic, exemple(1).matricule, x) == 1);
    x = chercher_personnel(fic, exemple(1).matricule);
    VERIF(strcmp(x.fonction, "Steward") == 0);
    VERIF(modifier_personnel(fic, "ZZZZ", x) == 0);
    x = exemple(1); strcpy(x.cin, exemple(0).cin);
    VERIF(modifier_personnel(fic, exemple(1).matricule, x) == -2);          /* valeur unique déjà prise */
    /* 5. filtre et tri */
    n = charger_personnel(fic, tab, MAX_PERSONNEL);
    k = filtrer_personnel(tab, n, res, "", 0, "", "");
    VERIF(k == 5);
    k = filtrer_personnel(tab, n, res, exemple(3).matricule, 0, "", "");
    VERIF(k == 1);
    k = filtrer_personnel(tab, n, res, exemple(3).matricule, 1, "", "");
    VERIF(k == 1);
    k = filtrer_personnel(tab, n, res, "", 0, "inexistant", "");
    VERIF(k == 0);
    trier_personnel(tab, n, 1);
    for (i = 1; i < n; i++) VERIF(strcasecmp(tab[i - 1].nom, tab[i].nom) <= 0);
    VERIF(sauvegarder_personnel("tri_personnel.txt", tab, n) == 1);
    trier_personnel(tab, n, 0);
    for (i = 1; i < n; i++) VERIF(strcasecmp(tab[i - 1].nom, tab[i].nom) >= 0);
    /* 6. comptage */
    n = charger_personnel(fic, tab, MAX_PERSONNEL);
    VERIF(compter_personnel(tab, n, "Disponible") >= 0);
    /* 7. données invalides */
    x = exemple(0); x.matricule[0] = '\0';
    VERIF(valider_personnel(x) == 1);
    x = exemple(0);
    strcpy(x.embauche, "10/11/2026"); strcpy(x.expiration, "01/01/2020");
    VERIF(valider_personnel(x) == 14);
    /* 7b. exportation et total */
    n = charger_personnel(fic, tab, MAX_PERSONNEL);
    VERIF(exporter_personnel("export_personnel.csv", tab, n) == 1);
    VERIF(exporter_personnel("/dossier/inexistant/x.csv", tab, n) == -1);
    VERIF(total_personnel(tab, n) > 0);
    VERIF(total_personnel(tab, 0) == 0);
    /* 8. suppression */
    VERIF(supprimer_personnel(fic, exemple(4).matricule) == 1);
    VERIF(supprimer_personnel(fic, exemple(4).matricule) == 0);
    n = charger_personnel(fic, tab, MAX_PERSONNEL);
    VERIF(n == 4);

    if (rapport) { fprintf(rapport, "%d verifications, %d echec(s)\n", total, echecs); fclose(rapport); }
    return echecs == 0 ? 0 : 1;
}
