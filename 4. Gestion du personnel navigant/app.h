#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>
#include <glade/glade.h>
#include "personnel.h"
#include "historique.h"

#define FICHIER_GLADE "gestion_personnel.glade"
#define FICHIER_DONNEES "personnel.txt"
#define FICHIER_HIST "historique.txt"
#define FICHIER_USERS "users.txt"

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
void  afficher_gros(GtkWidget *label, const char *texte);
void  afficher_nombre_gros(GtkWidget *label, int valeur);
void  regler_progression(GtkWidget *barre, int valeur, int total, const char *unite);
void  afficher_date_du_jour(GtkWidget *label);

/* ---- qui est connecté (défini dans login.c) ---------------------------- */
extern char utilisateur[40];      /* identifiant                                    */
extern char reference[40];        /* ex : matricule, identifiant passager... ("" pour l'administrateur) */
extern int  est_admin;            /* 1 = administrateur, 0 = utilisateur            */

/* ---- les fenêtres de l'application ------------------------------------- */
void ouvrir_login(void);          /* login.c                   */
void ouvrir_admin(void);          /* interface_admin.c         */
void ouvrir_utilisateur(void);    /* interface_utilisateur.c   */
void historiser(const char *ref, const char *action, const char *details);   /* ajoute une ligne à historique.txt */

#endif
