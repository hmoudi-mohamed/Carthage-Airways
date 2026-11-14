/* login.c - Fenêtre de connexion : mène à l'espace administrateur ou à l'espace personnel navigant */
#include <stdio.h>
#include <string.h>
#include "app.h"

char utilisateur[12] = "";

static GladeXML *xml_login;
#define W(id) glade_xml_get_widget(xml_login, id)

/* Cherche « identifiant;motdepasse;role » dans users.txt.
 * Retourne 1 si trouvé (le rôle est copié dans role), 0 sinon. */
static int verifier_utilisateur(const char *login, const char *mdp, char *role, int taille)
{
    FILE *f = fopen(FICHIER_USERS, "r");
    char ligne[100];
    int ok = 0;

    if (f == NULL) return 0;
    while (!ok && fgets(ligne, sizeof ligne, f) != NULL) {
        char *sep1, *sep2;
        ligne[strcspn(ligne, "\r\n")] = '\0';
        sep1 = strchr(ligne, ';');
        if (sep1 == NULL) continue;
        sep2 = strchr(sep1 + 1, ';');
        if (sep2 == NULL) continue;
        *sep1 = '\0';
        *sep2 = '\0';                       /* ligne = identifiant, sep1 + 1 = mot de passe, sep2 + 1 = rôle */
        if (strcmp(ligne, login) == 0 && strcmp(sep1 + 1, mdp) == 0) {
            copier_texte(role, taille, sep2 + 1);
            ok = 1;
        }
    }
    fclose(f);
    return ok;
}

void ouvrir_login(void)
{
    GtkWidget *fenetre;

    xml_login = glade_xml_new(FICHIER_GLADE, "window_login", NULL);
    if (xml_login == NULL) {
        fprintf(stderr, "Impossible d'ouvrir %s (lancez ./gestion_personnel depuis son dossier)\n", FICHIER_GLADE);
        exit(1);
    }
    glade_xml_signal_autoconnect(xml_login);
    fenetre = W("window_login");
    g_signal_connect(fenetre, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(fenetre);
    gtk_widget_grab_focus(W("entry_identifiant"));
}

/* ----------------------- gestionnaires de signaux ----------------------- */
void on_btn_connexion_clicked(GtkWidget *bouton, gpointer data)
{
    char login[40], mdp[40], role[20];
    (void)bouton; (void)data;

    copier_texte(login, sizeof login, gtk_entry_get_text(GTK_ENTRY(W("entry_identifiant"))));
    copier_texte(mdp, sizeof mdp, gtk_entry_get_text(GTK_ENTRY(W("entry_motdepasse"))));
    enlever_espaces(login);

    if (strcmp(login, "") == 0 || strcmp(mdp, "") == 0) {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_login")), "Saisissez l'identifiant et le mot de passe.");
        return;
    }
    if (!verifier_utilisateur(login, mdp, role, sizeof role)) {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_login")), "Identifiant ou mot de passe incorrect.");
        ecrire_entry(W("entry_motdepasse"), "");
        return;
    }

    if (strcmp(role, "admin") == 0) {                          /* espace administrateur */
        gtk_widget_destroy(W("window_login"));
        ouvrir_principal();
    } else {                                                   /* espace personnel navigant */
        Personnel p = chercher_personnel(FICHIER_PERSONNEL, login);
        if (p.matricule[0] == '\0') {
            gtk_label_set_text(GTK_LABEL(W("lbl_msg_login")), "Aucune fiche de personnel pour ce matricule.");
            return;
        }
        copier_texte(utilisateur, sizeof utilisateur, login);
        gtk_widget_destroy(W("window_login"));
        ouvrir_espace();
    }
}

void on_entry_identifiant_activate(GtkWidget *w, gpointer data)
{ (void)w; (void)data; gtk_widget_grab_focus(W("entry_motdepasse")); }

void on_entry_motdepasse_activate(GtkWidget *w, gpointer data)
{ on_btn_connexion_clicked(w, data); }

void on_btn_quitter_clicked(GtkWidget *w, gpointer data)
{ (void)w; (void)data; gtk_main_quit(); }
