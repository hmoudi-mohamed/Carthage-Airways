#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>
#include <glade/glade.h>
#include "personnel.h"
#include "avion.h"

#define FICHIER_GLADE "gestion_personnel.glade"
#define FICHIER_PERSONNEL "personnel.txt"
#define FICHIER_AVIONS "avions.txt"
#define FICHIER_USERS "users.txt"

/* matricule du membre du personnel connecté (rempli par login.c) */
extern char utilisateur[12];

/* ---- outils.c : petites fonctions d'aide pour l'interface -------------- */
void  copier_texte(char *dest, int taille, const char *source);
void  enlever_espaces(char *s);
void  remplir_combo(GtkWidget *combo, const char **choix, int n, int actif);
char *texte_combo(GtkWidget *combo);                 /* à libérer avec g_free ; "" si rien de choisi */
void  choisir_combo(GtkWidget *combo, const char *valeur);
void  ecrire_entry(GtkWidget *entry, const char *texte);
void  afficher_statut(GtkWidget *barre, const char *texte);
GtkListStore *preparer_liste(GtkWidget *treeview, int nb_colonnes, const char **titres);
int   valeur_selectionnee(GtkWidget *treeview, int colonne, char *dest, int taille);

/* ---- les fenêtres de l'application ------------------------------------- */
void ouvrir_login(void);       /* login.c               : connexion (2 espaces)              */
void ouvrir_principal(void);   /* principal.c           : menu de l'espace administrateur    */
void ouvrir_gestion(void);     /* gestion_personnel.c   : gestion du personnel navigant      */
void ouvrir_espace(void);      /* espace.c              : espace du personnel navigant       */

#endif
