/* principal.c - Interface principale : menu de l'espace administrateur */
#include <stdio.h>
#include "app.h"

static GladeXML *xml_principal;
#define W(id) glade_xml_get_widget(xml_principal, id)

void ouvrir_principal(void)
{
    GtkWidget *fenetre;

    xml_principal = glade_xml_new(FICHIER_GLADE, "window_principal", NULL);
    glade_xml_signal_autoconnect(xml_principal);
    fenetre = W("window_principal");
    g_signal_connect(fenetre, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    gtk_label_set_text(GTK_LABEL(W("lbl_user_menu")), "Connecté : admin");
    gtk_widget_show_all(fenetre);
}

/* ----------------------- gestionnaires de signaux ----------------------- */
void on_btn_gestion_personnel_clicked(GtkWidget *w, gpointer data)
{
    (void)w; (void)data;
    gtk_widget_destroy(W("window_principal"));
    ouvrir_gestion();
}

void on_btn_deconnexion_menu_clicked(GtkWidget *w, gpointer data)
{
    (void)w; (void)data;
    gtk_widget_destroy(W("window_principal"));
    ouvrir_login();
}

void on_btn_quitter_menu_clicked(GtkWidget *w, gpointer data)
{ (void)w; (void)data; gtk_main_quit(); }
