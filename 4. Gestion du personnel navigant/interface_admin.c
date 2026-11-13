/* interface_admin.c - Espace ADMINISTRATEUR de la gestion « Gestion du personnel navigant ».
 * Trois onglets : Gestion (ajout, modification, suppression, recherche, tri, filtres), Statistiques, Historique.
 * Toute la logique (fichier, recherche, tri, contrôle) est dans personnel.c et historique.c : ici, seulement l'interface. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"

static GladeXML *xml;
#define W(id) glade_xml_get_widget(xml, id)

static Personnel tab[MAX_PERSONNEL];            /* tous les éléments du fichier    */
static Personnel vue[MAX_PERSONNEL];            /* éléments affichés dans la liste */
static Historique hist[MAX_HIST], hvue[MAX_HIST];
static int nb = 0, nv = 0, nbh = 0, nvh = 0;
static int tri_croissant = 1;
static int pret = 0;                          /* 1 quand la fenêtre est entièrement initialisée */
static char cle_choisie[64] = "";             /* identifiant de la ligne sélectionnée */
static GtkListStore *liste, *liste_h;

static const char *c_fonction[] = { "Pilote", "Copilote", "Hôtesse", "Steward" };
static const char *c_base[] = { "Tunis-Carthage", "Djerba-Zarzis", "Monastir", "Sfax", "Tozeur" };
static const char *c_statut[] = { "Disponible", "En vol", "En congé" };
static const char *c_filtre1[] = { "Tous", "Pilote", "Copilote", "Hôtesse", "Steward" };
static const char *c_filtre2[] = { "Tous", "Disponible", "En vol", "En congé" };
static const char *t_liste[] = { "Matricule", "Nom", "Prénom", "Fonction", "Base d'attache", "Heures de vol", "Expiration licence", "Statut" };
static int correspond_case(Personnel *x) { return strcmp(x->statut, "En congé") == 0; }
static const char *choix_action[] = { "Toutes", "Ajout", "Modification", "Suppression", "Export" };
static const char *t_hist[] = { "Date et heure", "Utilisateur", "Référence", "Action", "Détails" };

/* Ajoute une ligne au journal historique.txt (utilisé aussi par l'espace utilisateur). */
void historiser(const char *ref, const char *action, const char *details)
{
    char r[12], a[16], d[80], u[20];
    copier_texte(r, sizeof r, ref);
    copier_texte(a, sizeof a, action);
    copier_texte(d, sizeof d, details);
    copier_texte(u, sizeof u, utilisateur);
    ajouter_historique(FICHIER_HIST, u, r, a, d);
}

/* ------------------------------------------------------------------ */
/*  Affichage                                                          */
/* ------------------------------------------------------------------ */
static void remplir_liste(void)
{
    int i, k, critere;
    char texte[64], info[80], b_heures[16];
    char *v1 = texte_combo(W("combo_filtre1")), *v2 = texte_combo(W("combo_filtre2"));

    critere = gtk_combo_box_get_active(GTK_COMBO_BOX(W("combo_critere")));
    if (critere < 0) critere = 0;
    if (strcmp(v1, "Tous") == 0) v1[0] = '\0';
    if (strcmp(v2, "Tous") == 0) v2[0] = '\0';
    copier_texte(texte, sizeof texte, gtk_entry_get_text(GTK_ENTRY(W("entry_recherche"))));
    enlever_espaces(texte);

    nv = filtrer_personnel(tab, nb, vue, texte, critere, v1, v2);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(W("chk_filtre")))) {
        k = 0;
        for (i = 0; i < nv; i++)
            if (correspond_case(&vue[i])) vue[k++] = vue[i];
        nv = k;
    }
    trier_personnel(vue, nv, tri_croissant);

    gtk_list_store_clear(liste);
    for (i = 0; i < nv; i++) {
        GtkTreeIter it;
        Personnel *x = &vue[i];
        snprintf(b_heures, sizeof b_heures, "%d", x->heures);
        gtk_list_store_append(liste, &it);
        gtk_list_store_set(liste, &it, 0, x->matricule, 1, x->nom, 2, x->prenom, 3, x->fonction, 4, x->base, 5, b_heures, 6, x->expiration, 7, x->statut, -1);
    }
    snprintf(info, sizeof info, "%d membres affichés sur %d", nv, nb);
    gtk_label_set_text(GTK_LABEL(W("lbl_nb")), info);
    g_free(v1);
    g_free(v2);
}

static void remplir_stats(void)
{
    int c;
    char b[24], info[100];

    afficher_nombre_gros(W("lbl_s_total"), nb);
    afficher_nombre_gros(W("lbl_s_sum"), (int)total_personnel(tab, nb));
    c = compter_personnel(tab, nb, "Disponible");
    snprintf(b, sizeof b, "%d", c);
    gtk_label_set_text(GTK_LABEL(W("lbl_s_n0")), b);
    regler_progression(W("bar_s_0"), c, nb, "membres");
    c = compter_personnel(tab, nb, "En vol");
    snprintf(b, sizeof b, "%d", c);
    gtk_label_set_text(GTK_LABEL(W("lbl_s_n1")), b);
    regler_progression(W("bar_s_1"), c, nb, "membres");
    c = compter_personnel(tab, nb, "En congé");
    snprintf(b, sizeof b, "%d", c);
    gtk_label_set_text(GTK_LABEL(W("lbl_s_n2")), b);
    regler_progression(W("bar_s_2"), c, nb, "membres");
    snprintf(info, sizeof info, "Mise à jour : lecture du fichier personnel.txt (%d lignes)", nb);
    gtk_label_set_text(GTK_LABEL(W("lbl_s_info")), info);
}

static void remplir_historique(void)
{
    int i;
    char texte[64], info[80];
    char *action = texte_combo(W("combo_h_action"));

    if (strcmp(action, "Toutes") == 0) action[0] = '\0';
    copier_texte(texte, sizeof texte, gtk_entry_get_text(GTK_ENTRY(W("entry_h_recherche"))));
    enlever_espaces(texte);
    nvh = filtrer_historique(hist, nbh, hvue, texte, action);
    gtk_list_store_clear(liste_h);
    for (i = nvh - 1; i >= 0; i--) {                 /* les actions les plus récentes en premier */
        GtkTreeIter it;
        gtk_list_store_append(liste_h, &it);
        gtk_list_store_set(liste_h, &it, 0, hvue[i].date, 1, hvue[i].utilisateur, 2, hvue[i].reference,
                           3, hvue[i].action, 4, hvue[i].details, -1);
    }
    snprintf(info, sizeof info, "%d actions affichées (sur %d)", nvh, nbh);
    gtk_label_set_text(GTK_LABEL(W("lbl_h_nb")), info);
    g_free(action);
}

static void recharger(void)
{
    nb = charger_personnel(FICHIER_DONNEES, tab, MAX_PERSONNEL);
    if (nb < 0) nb = 0;
    nbh = charger_historique(FICHIER_HIST, hist, MAX_HIST);
    if (nbh < 0) nbh = 0;
    remplir_liste();
    remplir_stats();
    remplir_historique();
}

/* ------------------------------------------------------------------ */
/*  Formulaire                                                         */
/* ------------------------------------------------------------------ */
static void vider_formulaire(void)
{
    cle_choisie[0] = '\0';
    ecrire_entry(W("f_matricule"), "");
    ecrire_entry(W("f_nom"), "");
    ecrire_entry(W("f_prenom"), "");
    ecrire_entry(W("f_cin"), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_f_fonction")), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_f_base")), 0);
    ecrire_entry(W("f_embauche"), "");
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(W("f_heures")), 0);
    ecrire_entry(W("f_licence"), "");
    ecrire_entry(W("f_expiration"), "");
    ecrire_entry(W("f_tel"), "");
    ecrire_entry(W("f_email"), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_f_statut")), 0);
    gtk_editable_set_editable(GTK_EDITABLE(W("f_matricule")), TRUE);
    gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "");
}

static void charger_formulaire(Personnel *x)
{
    copier_texte(cle_choisie, sizeof cle_choisie, x->matricule);
    ecrire_entry(W("f_matricule"), x->matricule);
    ecrire_entry(W("f_nom"), x->nom);
    ecrire_entry(W("f_prenom"), x->prenom);
    ecrire_entry(W("f_cin"), x->cin);
    choisir_combo(W("combo_f_fonction"), x->fonction);
    choisir_combo(W("combo_f_base"), x->base);
    ecrire_entry(W("f_embauche"), x->embauche);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(W("f_heures")), x->heures);
    ecrire_entry(W("f_licence"), x->licence);
    ecrire_entry(W("f_expiration"), x->expiration);
    ecrire_entry(W("f_tel"), x->tel);
    ecrire_entry(W("f_email"), x->email);
    choisir_combo(W("combo_f_statut"), x->statut);
    gtk_editable_set_editable(GTK_EDITABLE(W("f_matricule")), FALSE);     /* l'identifiant ne se modifie pas */
}

static void lire_formulaire(Personnel *x)
{
    char *t;
    memset(x, 0, sizeof *x);
    copier_texte(x->matricule, sizeof x->matricule, gtk_entry_get_text(GTK_ENTRY(W("f_matricule")))); enlever_espaces(x->matricule);
    copier_texte(x->nom, sizeof x->nom, gtk_entry_get_text(GTK_ENTRY(W("f_nom")))); enlever_espaces(x->nom);
    copier_texte(x->prenom, sizeof x->prenom, gtk_entry_get_text(GTK_ENTRY(W("f_prenom")))); enlever_espaces(x->prenom);
    copier_texte(x->cin, sizeof x->cin, gtk_entry_get_text(GTK_ENTRY(W("f_cin")))); enlever_espaces(x->cin);
    t = texte_combo(W("combo_f_fonction")); copier_texte(x->fonction, sizeof x->fonction, t); g_free(t);
    t = texte_combo(W("combo_f_base")); copier_texte(x->base, sizeof x->base, t); g_free(t);
    copier_texte(x->embauche, sizeof x->embauche, gtk_entry_get_text(GTK_ENTRY(W("f_embauche")))); enlever_espaces(x->embauche);
    x->heures = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(W("f_heures")));
    copier_texte(x->licence, sizeof x->licence, gtk_entry_get_text(GTK_ENTRY(W("f_licence")))); enlever_espaces(x->licence);
    copier_texte(x->expiration, sizeof x->expiration, gtk_entry_get_text(GTK_ENTRY(W("f_expiration")))); enlever_espaces(x->expiration);
    copier_texte(x->tel, sizeof x->tel, gtk_entry_get_text(GTK_ENTRY(W("f_tel")))); enlever_espaces(x->tel);
    copier_texte(x->email, sizeof x->email, gtk_entry_get_text(GTK_ENTRY(W("f_email")))); enlever_espaces(x->email);
    t = texte_combo(W("combo_f_statut")); copier_texte(x->statut, sizeof x->statut, t); g_free(t);
}

void ouvrir_admin(void)
{
    GtkWidget *fenetre;
    char b[80];

    pret = 0;
    xml = glade_xml_new(FICHIER_GLADE, "window_admin", NULL);
    glade_xml_signal_autoconnect(xml);
    fenetre = W("window_admin");
    g_signal_connect(fenetre, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
    remplir_combo(W("combo_f_fonction"), c_fonction, 4, 0);
    remplir_combo(W("combo_f_base"), c_base, 5, 0);
    remplir_combo(W("combo_f_statut"), c_statut, 3, 0);
    remplir_combo(W("combo_filtre1"), c_filtre1, 5, 0);
    remplir_combo(W("combo_filtre2"), c_filtre2, 4, 0);
    remplir_combo(W("combo_critere"), CRITERES_PERSONNEL, NB_CRITERES_PERSONNEL, 0);
    remplir_combo(W("combo_h_action"), choix_action, 5, 0);
    liste = preparer_liste(W("treeview_liste"), 8, t_liste);
    liste_h = preparer_liste(W("treeview_hist"), 5, t_hist);
    snprintf(b, sizeof b, "Connecté : %s", utilisateur);
    gtk_label_set_text(GTK_LABEL(W("lbl_user_admin")), b);
    afficher_date_du_jour(W("lbl_date_ad"));
    recharger();
    vider_formulaire();
    pret = 1;
    afficher_statut(W("statusbar_admin"), "Connecté en tant qu'administrateur.");
    gtk_widget_show_all(fenetre);
}

/* ------------------------------------------------------------------ */
/*  Gestionnaires de signaux : onglet Gestion                          */
/* ------------------------------------------------------------------ */
void on_btn_ajouter_clicked(GtkWidget *w, gpointer d)
{
    Personnel x;
    int code, r;
    GtkWidget *lbl = W("lbl_msg_fiche");
    char details[80];
    (void)w; (void)d;
    lire_formulaire(&x);
    code = valider_personnel(x);
    if (code) { gtk_label_set_text(GTK_LABEL(lbl), message_erreur_personnel(code)); return; }
    r = ajouter_personnel(FICHIER_DONNEES, x);
    if (r == 1) {
        snprintf(details, sizeof details, "Ajout dans personnel.txt");
        historiser(x.matricule, "Ajout", details);
        recharger();
        vider_formulaire();
        gtk_label_set_text(GTK_LABEL(lbl), "Ajout effectué : le fichier texte a été mis à jour.");
    }
    else if (r == 0) gtk_label_set_text(GTK_LABEL(lbl), "Cet identifiant existe déjà.");
    else if (r == -2) gtk_label_set_text(GTK_LABEL(lbl), "Une valeur qui doit être unique (ex : CIN) est déjà utilisée.");
    else gtk_label_set_text(GTK_LABEL(lbl), "Erreur d'accès au fichier.");
}

void on_btn_modifier_clicked(GtkWidget *w, gpointer d)
{
    Personnel x;
    int code, r;
    GtkWidget *lbl = W("lbl_msg_fiche");
    (void)w; (void)d;
    if (!cle_choisie[0]) { gtk_label_set_text(GTK_LABEL(lbl), "Sélectionnez d'abord une ligne dans la liste."); return; }
    lire_formulaire(&x);
    code = valider_personnel(x);
    if (code) { gtk_label_set_text(GTK_LABEL(lbl), message_erreur_personnel(code)); return; }
    r = modifier_personnel(FICHIER_DONNEES, cle_choisie, x);
    if (r == 1) {
        historiser(cle_choisie, "Modification", "Fiche modifiée");
        recharger();
        gtk_label_set_text(GTK_LABEL(lbl), "Modification enregistrée dans le fichier texte.");
    }
    else if (r == -2) gtk_label_set_text(GTK_LABEL(lbl), "Une valeur qui doit être unique (ex : CIN) est déjà utilisée.");
    else if (r == 0) gtk_label_set_text(GTK_LABEL(lbl), "Élément introuvable.");
    else gtk_label_set_text(GTK_LABEL(lbl), "Erreur d'accès au fichier.");
}

void on_btn_supprimer_clicked(GtkWidget *w, gpointer d)
{
    GtkWidget *lbl = W("lbl_msg_fiche"), *dlg;
    int rep;
    char ancien[64];
    (void)w; (void)d;
    if (!cle_choisie[0]) { gtk_label_set_text(GTK_LABEL(lbl), "Sélectionnez d'abord une ligne dans la liste."); return; }
    dlg = gtk_message_dialog_new(GTK_WINDOW(W("window_admin")), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
                                 "Supprimer « %s » ?", cle_choisie);
    rep = gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
    if (rep != GTK_RESPONSE_YES) return;
    copier_texte(ancien, sizeof ancien, cle_choisie);
    if (supprimer_personnel(FICHIER_DONNEES, ancien) == 1) {
        historiser(ancien, "Suppression", "Élément supprimé");
        recharger();
        vider_formulaire();
        gtk_label_set_text(GTK_LABEL(lbl), "Suppression effectuée.");
    }
    else gtk_label_set_text(GTK_LABEL(lbl), "Suppression impossible.");
}

void on_btn_vider_clicked(GtkWidget *w, gpointer d)
{ (void)w; (void)d; vider_formulaire(); }

void on_btn_exporter_clicked(GtkWidget *w, gpointer d)
{
    (void)w; (void)d;
    if (exporter_personnel("export_personnel.csv", vue, nv) == 1) {
        historiser("-", "Export", "export_personnel.csv");
        recharger();
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Liste exportée dans export_personnel.csv (ouvrable avec Excel).");
    }
    else gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Exportation impossible.");
}

void on_treeview_liste_cursor_changed(GtkWidget *tv, gpointer d)
{
    char cle[64];
    Personnel x;
    (void)d;
    if (!valeur_selectionnee(tv, 0, cle, sizeof cle)) return;
    x = chercher_personnel(FICHIER_DONNEES, cle);
    if (x.matricule[0]) {
        charger_formulaire(&x);
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "");
    }
}

void on_btn_rechercher_clicked(GtkWidget *w, gpointer d)
{ (void)w; (void)d; remplir_liste(); afficher_statut(W("statusbar_admin"), "Recherche effectuée."); }

void on_entry_recherche_activate(GtkWidget *w, gpointer d)
{ on_btn_rechercher_clicked(w, d); }

void on_btn_reinitialiser_clicked(GtkWidget *w, gpointer d)
{
    (void)w; (void)d;
    pret = 0;
    ecrire_entry(W("entry_recherche"), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_critere")), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_filtre1")), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_filtre2")), 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(W("chk_filtre")), FALSE);
    pret = 1;
    remplir_liste();
    afficher_statut(W("statusbar_admin"), "Filtres réinitialisés.");
}

void on_rb_tri_asc_toggled(GtkWidget *w, gpointer d)
{ (void)d; if (!pret || !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) return; tri_croissant = 1; remplir_liste(); }
void on_rb_tri_desc_toggled(GtkWidget *w, gpointer d)
{ (void)d; if (!pret || !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) return; tri_croissant = 0; remplir_liste(); }
void on_combo_filtre1_changed(GtkWidget *w, gpointer d) { (void)w; (void)d; if (pret) remplir_liste(); }
void on_combo_filtre2_changed(GtkWidget *w, gpointer d) { (void)w; (void)d; if (pret) remplir_liste(); }
void on_chk_filtre_toggled(GtkWidget *w, gpointer d) { (void)w; (void)d; if (pret) remplir_liste(); }

/* ------------------------------------------------------------------ */
/*  Gestionnaires de signaux : onglet Historique                       */
/* ------------------------------------------------------------------ */
void on_btn_h_rechercher_clicked(GtkWidget *w, gpointer d)
{ (void)w; (void)d; remplir_historique(); }
void on_entry_h_recherche_activate(GtkWidget *w, gpointer d)
{ on_btn_h_rechercher_clicked(w, d); }
void on_combo_h_action_changed(GtkWidget *w, gpointer d)
{ (void)w; (void)d; if (pret) remplir_historique(); }

void on_btn_h_reinitialiser_clicked(GtkWidget *w, gpointer d)
{
    (void)w; (void)d;
    pret = 0;
    ecrire_entry(W("entry_h_recherche"), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_h_action")), 0);
    pret = 1;
    remplir_historique();
}

void on_btn_h_exporter_clicked(GtkWidget *w, gpointer d)
{
    (void)w; (void)d;
    if (exporter_historique("export_historique.csv", hvue, nvh) == 1)
        afficher_statut(W("statusbar_admin"), "Historique exporté dans export_historique.csv.");
    else afficher_statut(W("statusbar_admin"), "Exportation impossible.");
}

void on_btn_h_vider_clicked(GtkWidget *w, gpointer d)
{
    GtkWidget *dlg;
    int rep;
    (void)w; (void)d;
    dlg = gtk_message_dialog_new(GTK_WINDOW(W("window_admin")), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO,
                                 "Effacer tout l'historique ?");
    rep = gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
    if (rep != GTK_RESPONSE_YES) return;
    vider_historique(FICHIER_HIST);
    recharger();
    afficher_statut(W("statusbar_admin"), "Historique effacé.");
}

void on_btn_deconnexion_admin_clicked(GtkWidget *w, gpointer d)
{
    (void)w; (void)d;
    gtk_widget_destroy(W("window_admin"));
    ouvrir_login();
}
