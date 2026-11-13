/* interface_utilisateur.c - Espace UTILISATEUR (PERSONNEL NAVIGANT) de la gestion « Gestion du personnel navigant ».
 * L'utilisateur peut chercher, consulter les détails et modifier certaines informations. Il n'a aucun droit d'ajout ni de suppression
 * (ces actions sont réservées à l'administrateur). La logique est dans personnel.c. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"

static GladeXML *xml;
#define W(id) glade_xml_get_widget(xml, id)

static Personnel tab[MAX_PERSONNEL], vue[MAX_PERSONNEL];
static int nb = 0, nv = 0, pret = 0;
static char cle[64] = "";                 /* identifiant de la ligne sélectionnée */
static GtkListStore *liste;

static const char *t_liste[] = { "Matricule", "Nom", "Prénom", "Fonction", "Base d'attache", "Heures de vol", "Statut" };

static void afficher_details(Personnel *x)
{
    char b[24];
    gtk_label_set_text(GTK_LABEL(W("u_val_matricule")), x->matricule[0] ? x->matricule : "--");
    gtk_label_set_text(GTK_LABEL(W("u_val_nom")), x->nom[0] ? x->nom : "--");
    gtk_label_set_text(GTK_LABEL(W("u_val_prenom")), x->prenom[0] ? x->prenom : "--");
    gtk_label_set_text(GTK_LABEL(W("u_val_cin")), x->cin[0] ? x->cin : "--");
    gtk_label_set_text(GTK_LABEL(W("u_val_fonction")), x->fonction[0] ? x->fonction : "--");
    gtk_label_set_text(GTK_LABEL(W("u_val_base")), x->base[0] ? x->base : "--");
    gtk_label_set_text(GTK_LABEL(W("u_val_embauche")), x->embauche[0] ? x->embauche : "--");
    snprintf(b, sizeof b, "%d", x->heures);
    gtk_label_set_text(GTK_LABEL(W("u_val_heures")), b);
    gtk_label_set_text(GTK_LABEL(W("u_val_licence")), x->licence[0] ? x->licence : "--");
    gtk_label_set_text(GTK_LABEL(W("u_val_expiration")), x->expiration[0] ? x->expiration : "--");
    gtk_label_set_text(GTK_LABEL(W("u_val_tel")), x->tel[0] ? x->tel : "--");
    gtk_label_set_text(GTK_LABEL(W("u_val_email")), x->email[0] ? x->email : "--");
    gtk_label_set_text(GTK_LABEL(W("u_val_statut")), x->statut[0] ? x->statut : "--");
    if (1) {
    ecrire_entry(W("u_f_tel"), x->tel);
    ecrire_entry(W("u_f_email"), x->email);
        gtk_label_set_text(GTK_LABEL(W("lbl_u_msg")), "");
    }
}

static void remplir_liste(void)
{
    int i, k, critere;
    char texte[64], info[80], b_heures[16];

    critere = gtk_combo_box_get_active(GTK_COMBO_BOX(W("combo_u_critere")));
    if (critere < 0) critere = 0;
    copier_texte(texte, sizeof texte, gtk_entry_get_text(GTK_ENTRY(W("entry_u_recherche"))));
    enlever_espaces(texte);
    nv = filtrer_personnel(tab, nb, vue, texte, critere, "", "");
    if (reference[0]) {                      /* l'utilisateur ne voit que SES données */
        k = 0;
        for (i = 0; i < nv; i++)
            if (strcmp(vue[i].matricule, reference) == 0) vue[k++] = vue[i];
        nv = k;
    }
    trier_personnel(vue, nv, 1);
    gtk_list_store_clear(liste);
    for (i = 0; i < nv; i++) {
        GtkTreeIter it;
        Personnel *x = &vue[i];
        snprintf(b_heures, sizeof b_heures, "%d", x->heures);
        gtk_list_store_append(liste, &it);
        gtk_list_store_set(liste, &it, 0, x->matricule, 1, x->nom, 2, x->prenom, 3, x->fonction, 4, x->base, 5, b_heures, 6, x->statut, -1);
    }
    snprintf(info, sizeof info, "%d résultat(s)", nv);
    gtk_label_set_text(GTK_LABEL(W("lbl_u_nb")), info);
}

static void recharger(void)
{
    nb = charger_personnel(FICHIER_DONNEES, tab, MAX_PERSONNEL);
    if (nb < 0) nb = 0;
    remplir_liste();
}

void ouvrir_utilisateur(void)
{
    GtkWidget *fenetre;
    char b[100];

    pret = 0;
    cle[0] = '\0';
    xml = glade_xml_new(FICHIER_GLADE, "window_user", NULL);
    glade_xml_signal_autoconnect(xml);
    fenetre = W("window_user");
    g_signal_connect(fenetre, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    remplir_combo(W("combo_u_critere"), CRITERES_PERSONNEL, NB_CRITERES_PERSONNEL, 0);
    liste = preparer_liste(W("treeview_u"), 7, t_liste);
    snprintf(b, sizeof b, "Connecté : %s", utilisateur);
    gtk_label_set_text(GTK_LABEL(W("lbl_user_user")), b);
    afficher_date_du_jour(W("lbl_date_us"));
    recharger();
    pret = 1;
    afficher_statut(W("statusbar_user"), "Espace personnel navigant - consultation.");
    gtk_widget_show_all(fenetre);
    if (reference[0] && nv > 0) {                       /* un seul dossier : on l'affiche tout de suite */
        copier_texte(cle, sizeof cle, vue[0].matricule);
        afficher_details(&vue[0]);
    }
}

/* ----------------------- gestionnaires de signaux ----------------------- */
void on_treeview_u_cursor_changed(GtkWidget *tv, gpointer d)
{
    char c[64];
    Personnel x;
    (void)d;
    if (!valeur_selectionnee(tv, 0, c, sizeof c)) return;
    x = chercher_personnel(FICHIER_DONNEES, c);
    if (x.matricule[0]) {
        copier_texte(cle, sizeof cle, c);
        afficher_details(&x);
    }
}

void on_btn_u_rechercher_clicked(GtkWidget *w, gpointer d)
{ (void)w; (void)d; if (pret) remplir_liste(); afficher_statut(W("statusbar_user"), "Recherche effectuée."); }

void on_entry_u_recherche_activate(GtkWidget *w, gpointer d)
{ on_btn_u_rechercher_clicked(w, d); }

void on_btn_u_reinitialiser_clicked(GtkWidget *w, gpointer d)
{
    (void)w; (void)d;
    ecrire_entry(W("entry_u_recherche"), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_u_critere")), 0);
    remplir_liste();
    afficher_statut(W("statusbar_user"), "Recherche réinitialisée.");
}

void on_btn_u_enregistrer_clicked(GtkWidget *w, gpointer d)
{
    Personnel x;
    int code, r;
    GtkWidget *lbl = W("lbl_u_msg");
    (void)w; (void)d;
    if (!cle[0]) { gtk_label_set_text(GTK_LABEL(lbl), "Sélectionnez d'abord une ligne dans la liste."); return; }
    x = chercher_personnel(FICHIER_DONNEES, cle);
    if (!x.matricule[0]) { gtk_label_set_text(GTK_LABEL(lbl), "Élément introuvable."); return; }
    copier_texte(x.tel, sizeof x.tel, gtk_entry_get_text(GTK_ENTRY(W("u_f_tel")))); enlever_espaces(x.tel);
    copier_texte(x.email, sizeof x.email, gtk_entry_get_text(GTK_ENTRY(W("u_f_email")))); enlever_espaces(x.email);
    code = valider_personnel(x);
    if (code) { gtk_label_set_text(GTK_LABEL(lbl), message_erreur_personnel(code)); return; }
    r = modifier_personnel(FICHIER_DONNEES, cle, x);
    if (r == 1) {
        historiser(cle, "Modification", "Modifié par l'utilisateur");
        recharger();
        afficher_details(&x);
        gtk_label_set_text(GTK_LABEL(lbl), "Modifications enregistrées dans le fichier texte.");
    }
    else gtk_label_set_text(GTK_LABEL(lbl), "Enregistrement impossible.");
}

void on_btn_deconnexion_user_clicked(GtkWidget *w, gpointer d)
{
    (void)w; (void)d;
    gtk_widget_destroy(W("window_user"));
    ouvrir_login();
}
