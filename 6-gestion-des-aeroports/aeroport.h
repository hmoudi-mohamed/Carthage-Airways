#ifndef AEROPORT_H_INCLUDED
#define AEROPORT_H_INCLUDED
#include <stdio.h>
typedef struct
{
    char code_iata [4];
    char nom [50];
    char ville [30];
    char pays [30];
    int id, ouvert, international, capacite;
} aeroport;
//le type de retour int c'est pour indiquer si la tache a ete realisee avec succes ou non// pour erreur
//d'ouverture de fichier, element introuvable
int ajouter(char *, aeroport );
int modifier( char *, int, aeroport );
int supprimer(char *, int );
aeroport chercher(char *, int);

/* ================= Fonctions avancees ================= */
#define MAX_AEROPORTS 200

/* controle_saisie : 1 = saisie valide, sinon un code d'erreur negatif
   -1 id invalide (doit etre > 0)        -5 pays invalide
   -2 code IATA (3 lettres majuscules)   -6 ouvert (0 ou 1)
   -3 nom invalide (vide ou avec espace) -7 international (0 ou 1)
   -4 ville invalide                     -8 capacite (de 1 a 500)            */
int texte_valide(char *);
int controle_saisie(aeroport);

/* chercher_code : comme chercher, mais par code IATA (id = -1 si introuvable) */
aeroport chercher_code(char *, char *);

/* ajouter_controle / modifier_controle : controle de saisie + unicite
   1 = succes ; 0 = erreur de fichier ou aeroport introuvable ;
   -1 a -8 = erreur de saisie (voir ci-dessus) ;
   -9 = id deja existant ; -10 = code IATA deja existant                      */
int ajouter_controle(char *, aeroport);
int modifier_controle(char *, int, aeroport);

/* Les 3 fonctions suivantes ecrivent le resultat dans un fichier texte
   et renvoient le nombre d'aeroports ecrits (-1 si erreur de fichier).
   rechercher : critere 1 = code IATA, 2 = ville, 3 = pays, 4 = nom
   filtrer    : pays "*" = tous ; etat -1 = tous (0 ferme, 1 ouvert) ;
                international -1 = tous (0 non, 1 oui)
   trier      : ordre 1 = nom A->Z, 2 = nom Z->A                              */
int rechercher(char *, char *, int, char *);
int filtrer(char *, char *, char *, int, int);
int trier(char *, char *, int);

#endif // AEROPORT_H_INCLUDED
