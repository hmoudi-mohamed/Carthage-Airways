/* gestion_personnel.c - Fenêtre « Gestion du personnel navigant » (administrateur).
 * Toute la logique (fichier, recherche, tri, contrôle) est dans personnel.c :
 * ici on s'occupe seulement de l'interface. */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "app.h"

static GladeXML *xml_gest;
#define W(id) glade_xml_get_widget(xml_gest, id)

static Personnel tab[MAX_PERSONNEL];    /* tout le personnel du fichier        */
static Personnel vue[MAX_PERSONNEL];    /* membres affichés dans la liste      */
static int nb = 0, nv = 0;
static int tri_croissant = 1;
static int pret = 0;                    /* 1 quand la fenêtre est entièrement initialisée */
static char mat_choisi[12] = "";        /* matricule de la ligne sélectionnée  */
static GtkListStore *liste;

static const char *choix_fonction[]  = { "Pilote", "Copilote", "Hôtesse", "Steward" };
static const char *choix_base[]      = { "Tunis-Carthage", "Djerba-Zarzis", "Monastir", "Sfax", "Tozeur" };
static const char *choix_crit[]      = { "Tous les champs", "Matricule", "Nom", "Prénom", "Base d'attache" };
static const char *filtre_fonction[] = { "Toutes", "Pilote", "Copilote", "Hôtesse", "Steward" };
static const char *filtre_statut[]   = { "Tous", "Disponible", "En vol", "En congé" };
static const char *titres[] = { "Matricule", "Nom", "Prénom", "Fonction", "Base d'attache", "Statut",
                                "Heures de vol", "Expiration licence", "Téléphone" };

/* ------------------------------------------------------------------ */
/*  Affichage de la liste et des statistiques                          */
/* ------------------------------------------------------------------ */
static void afficher_liste(void)
{
    int i, k, critere;
    char texte[64], info[80], heures[16], exp[16];
    char *fonction = texte_combo(W("combo_filtre_fonction"));
    char *statut = texte_combo(W("combo_filtre_statut"));

    critere = gtk_combo_box_get_active(GTK_COMBO_BOX(W("combo_critere")));
    if (critere < 0) critere = 0;
    if (strcmp(fonction, "Toutes") == 0) fonction[0] = '\0';
    if (strcmp(statut, "Tous") == 0) statut[0] = '\0';
    copier_texte(texte, sizeof texte, gtk_entry_get_text(GTK_ENTRY(W("entry_recherche"))));
    enlever_espaces(texte);

    nv = filtrer_personnel(tab, nb, vue, texte, critere, fonction, statut);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(W("chk_licence")))) {
        k = 0;                              /* licences qui expirent dans 90 jours ou moins (ou déjà expirées) */
        for (i = 0; i < nv; i++)
            if (jours_restants(vue[i].expiration) <= 90) vue[k++] = vue[i];
        nv = k;
    }
    trier_personnel(vue, nv, tri_croissant);

    gtk_list_store_clear(liste);
    for (i = 0; i < nv; i++) {
        GtkTreeIter it;
        snprintf(heures, sizeof heures, "%d", vue[i].heures);
        snprintf(exp, sizeof exp, "%02d/%02d/%04d", vue[i].expiration.jour, vue[i].expiration.mois, vue[i].expiration.annee);
        gtk_list_store_append(liste, &it);
        gtk_list_store_set(liste, &it, 0, vue[i].matricule, 1, vue[i].nom, 2, vue[i].prenom, 3, vue[i].fonction,
                           4, vue[i].base, 5, vue[i].statut, 6, heures, 7, exp, 8, vue[i].tel, -1);
    }
    snprintf(info, sizeof info, "%d membres affichés", nv);
    gtk_label_set_text(GTK_LABEL(W("lbl_nb")), info);
    g_free(fonction);
    g_free(statut);
}

static void afficher_statistiques(void)
{
    int i, alertes = 0;
    char b[40];

    for (i = 0; i < nb; i++)
        if (jours_restants(tab[i].expiration) <= 90) alertes++;
    snprintf(b, sizeof b, "%d", nb);                                    gtk_label_set_text(GTK_LABEL(W("lbl_st_total")), b);
    snprintf(b, sizeof b, "%d", compter_fonction(tab, nb, "Pilote"));   gtk_label_set_text(GTK_LABEL(W("lbl_st_pilotes")), b);
    snprintf(b, sizeof b, "%d", compter_fonction(tab, nb, "Copilote")); gtk_label_set_text(GTK_LABEL(W("lbl_st_copilotes")), b);
    snprintf(b, sizeof b, "%d", compter_fonction(tab, nb, "Hôtesse"));  gtk_label_set_text(GTK_LABEL(W("lbl_st_hotesses")), b);
    snprintf(b, sizeof b, "%d", compter_fonction(tab, nb, "Steward"));  gtk_label_set_text(GTK_LABEL(W("lbl_st_stewards")), b);
    snprintf(b, sizeof b, "%d", compter_statut(tab, nb, "Disponible")); gtk_label_set_text(GTK_LABEL(W("lbl_st_dispo")), b);
    snprintf(b, sizeof b, "%d", compter_statut(tab, nb, "En vol"));     gtk_label_set_text(GTK_LABEL(W("lbl_st_envol")), b);
    snprintf(b, sizeof b, "%d", compter_statut(tab, nb, "En congé"));   gtk_label_set_text(GTK_LABEL(W("lbl_st_conge")), b);
    snprintf(b, sizeof b, "%d", alertes);                               gtk_label_set_text(GTK_LABEL(W("lbl_st_alertes")), b);
}

/* Relit le fichier texte puis met tout à jour. */
static void recharger(void)
{
    nb = charger_personnel(FICHIER_PERSONNEL, tab, MAX_PERSONNEL);
    if (nb < 0) nb = 0;
    afficher_liste();
    afficher_statistiques();
}

/* ------------------------------------------------------------------ */
/*  Formulaire                                                         */
/* ------------------------------------------------------------------ */
static void spin_date(const char *j, const char *m, const char *a, Date d)
{
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(W(j)), d.jour);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(W(m)), d.mois);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(W(a)), d.annee);
}

static Date lire_date(const char *j, const char *m, const char *a)
{
    Date d;
    d.jour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(W(j)));
    d.mois = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(W(m)));
    d.annee = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(W(a)));
    return d;
}

static void vider_formulaire(void)
{
    Date aujourdhui = {1, 1, 2026};
    mat_choisi[0] = '\0';
    ecrire_entry(W("f_matricule"), "");
    gtk_editable_set_editable(GTK_EDITABLE(W("f_matricule")), TRUE);
    ecrire_entry(W("f_nom"), "");
    ecrire_entry(W("f_prenom"), "");
    ecrire_entry(W("f_cin"), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_f_fonction")), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_f_base")), -1);
    spin_date("f_emb_j", "f_emb_m", "f_emb_a", aujourdhui);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(W("f_heures")), 0);
    ecrire_entry(W("f_licence"), "");
    spin_date("f_exp_j", "f_exp_m", "f_exp_a", aujourdhui);
    ecrire_entry(W("f_tel"), "");
    ecrire_entry(W("f_email"), "");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(W("rb_disponible")), TRUE);
    gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "");
}

static void remplir_formulaire(Personnel p)
{
    copier_texte(mat_choisi, sizeof mat_choisi, p.matricule);
    ecrire_entry(W("f_matricule"), p.matricule);
    gtk_editable_set_editable(GTK_EDITABLE(W("f_matricule")), FALSE);    /* le matricule ne se modifie pas */
    ecrire_entry(W("f_nom"), p.nom);
    ecrire_entry(W("f_prenom"), p.prenom);
    ecrire_entry(W("f_cin"), p.cin);
    choisir_combo(W("combo_f_fonction"), p.fonction);
    choisir_combo(W("combo_f_base"), p.base);
    spin_date("f_emb_j", "f_emb_m", "f_emb_a", p.embauche);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(W("f_heures")), p.heures);
    ecrire_entry(W("f_licence"), p.licence);
    spin_date("f_exp_j", "f_exp_m", "f_exp_a", p.expiration);
    ecrire_entry(W("f_tel"), p.tel);
    ecrire_entry(W("f_email"), p.email);
    if (strcmp(p.statut, "En vol") == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(W("rb_envol")), TRUE);
    else if (strcmp(p.statut, "En congé") == 0)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(W("rb_conge")), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(W("rb_disponible")), TRUE);
}

static void lire_texte(char *dest, int taille, const char *id)
{
    copier_texte(dest, taille, gtk_entry_get_text(GTK_ENTRY(W(id))));
    enlever_espaces(dest);
}

/* Lit les champs du formulaire et remplit une structure Personnel. */
static Personnel lire_formulaire(void)
{
    Personnel p;
    char *t;
    int i;

    memset(&p, 0, sizeof p);
    lire_texte(p.matricule, sizeof p.matricule, "f_matricule");
    for (i = 0; p.matricule[i]; i++) p.matricule[i] = toupper((unsigned char)p.matricule[i]);
    lire_texte(p.nom, sizeof p.nom, "f_nom");
    lire_texte(p.prenom, sizeof p.prenom, "f_prenom");
    lire_texte(p.cin, sizeof p.cin, "f_cin");
    t = texte_combo(W("combo_f_fonction"));
    copier_texte(p.fonction, sizeof p.fonction, t);
    g_free(t);
    t = texte_combo(W("combo_f_base"));
    copier_texte(p.base, sizeof p.base, t);
    g_free(t);
    p.embauche = lire_date("f_emb_j", "f_emb_m", "f_emb_a");
    p.heures = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(W("f_heures")));
    lire_texte(p.licence, sizeof p.licence, "f_licence");
    p.expiration = lire_date("f_exp_j", "f_exp_m", "f_exp_a");
    lire_texte(p.tel, sizeof p.tel, "f_tel");
    lire_texte(p.email, sizeof p.email, "f_email");
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(W("rb_disponible")))) strcpy(p.statut, "Disponible");
    else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(W("rb_envol")))) strcpy(p.statut, "En vol");
    else strcpy(p.statut, "En congé");
    return p;
}

/* ------------------------------------------------------------------ */
/*  Ouverture de la fenêtre                                            */
/* ------------------------------------------------------------------ */
void ouvrir_gestion(void)
{
    GtkWidget *fenetre;

    pret = 0;
    xml_gest = glade_xml_new(FICHIER_GLADE, "window_gestion", NULL);
    glade_xml_signal_autoconnect(xml_gest);
    fenetre = W("window_gestion");
    g_signal_connect(fenetre, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    remplir_combo(W("combo_f_fonction"), choix_fonction, 4, -1);
    remplir_combo(W("combo_f_base"), choix_base, 5, -1);
    remplir_combo(W("combo_critere"), choix_crit, 5, 0);
    remplir_combo(W("combo_filtre_fonction"), filtre_fonction, 5, 0);
    remplir_combo(W("combo_filtre_statut"), filtre_statut, 4, 0);
    liste = preparer_liste(W("treeview_personnel"), 9, titres);
    gtk_label_set_text(GTK_LABEL(W("lbl_user_gest")), "Connecté : admin");

    tri_croissant = 1;
    recharger();
    vider_formulaire();
    pret = 1;
    afficher_statut(W("statusbar_gest"), "Gestion du personnel navigant : les données sont dans personnel.txt");
    gtk_widget_show_all(fenetre);
}

/* ------------------------------------------------------------------ */
/*  Gestionnaires de signaux (noms donnés par Glade : on_<widget>_<signal>) */
/* ------------------------------------------------------------------ */
void on_btn_ajouter_clicked(GtkWidget *w, gpointer data)
{
    Personnel p;
    int erreur, r;
    (void)w; (void)data;

    p = lire_formulaire();
    erreur = valider_personnel(p);
    if (erreur != 0) {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), message_erreur_personnel(erreur));
        return;
    }
    r = ajouter_personnel(FICHIER_PERSONNEL, p);
    if (r == 1) {
        recharger();
        vider_formulaire();
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Membre ajouté : le fichier personnel.txt a été mis à jour.");
    } else if (r == 0) {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Ce matricule existe déjà.");
    } else if (r == -2) {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Ce CIN est déjà utilisé par un autre membre.");
    } else {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Erreur d'accès au fichier personnel.txt.");
    }
}

void on_btn_modifier_clicked(GtkWidget *w, gpointer data)
{
    Personnel p;
    int erreur, r;
    (void)w; (void)data;

    if (mat_choisi[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Sélectionnez d'abord un membre dans la liste.");
        return;
    }
    p = lire_formulaire();
    erreur = valider_personnel(p);
    if (erreur != 0) {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), message_erreur_personnel(erreur));
        return;
    }
    r = modifier_personnel(FICHIER_PERSONNEL, mat_choisi, p);
    if (r == 1) {
        recharger();
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Modification enregistrée dans le fichier.");
    } else if (r == -2) {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Ce CIN est déjà utilisé par un autre membre.");
    } else if (r == 0) {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Membre introuvable.");
    } else {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Erreur d'accès au fichier personnel.txt.");
    }
}

void on_btn_supprimer_clicked(GtkWidget *w, gpointer data)
{
    GtkWidget *dialogue;
    int reponse;
    (void)w; (void)data;

    if (mat_choisi[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Sélectionnez d'abord un membre dans la liste.");
        return;
    }
    dialogue = gtk_message_dialog_new(GTK_WINDOW(W("window_gestion")), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO, "Supprimer le membre %s ?", mat_choisi);
    reponse = gtk_dialog_run(GTK_DIALOG(dialogue));
    gtk_widget_destroy(dialogue);
    if (reponse != GTK_RESPONSE_YES) return;

    if (supprimer_personnel(FICHIER_PERSONNEL, mat_choisi) == 1) {
        recharger();
        vider_formulaire();
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Membre supprimé du fichier.");
    } else {
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "Suppression impossible.");
    }
}

void on_btn_vider_clicked(GtkWidget *w, gpointer data)
{ (void)w; (void)data; vider_formulaire(); }

/* Clic sur une ligne de la liste : on remplit le formulaire. */
void on_treeview_personnel_cursor_changed(GtkWidget *tv, gpointer data)
{
    char mat[12];
    Personnel p;
    (void)data;

    if (!valeur_selectionnee(tv, 0, mat, sizeof mat)) return;
    p = chercher_personnel(FICHIER_PERSONNEL, mat);
    if (p.matricule[0] != '\0') {
        remplir_formulaire(p);
        gtk_label_set_text(GTK_LABEL(W("lbl_msg_fiche")), "");
    }
}

void on_btn_rechercher_clicked(GtkWidget *w, gpointer data)
{ (void)w; (void)data; afficher_liste(); }

void on_entry_recherche_activate(GtkWidget *w, gpointer data)
{ on_btn_rechercher_clicked(w, data); }

void on_btn_reinitialiser_clicked(GtkWidget *w, gpointer data)
{
    (void)w; (void)data;
    pret = 0;                                    /* évite de réafficher plusieurs fois */
    ecrire_entry(W("entry_recherche"), "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_critere")), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_filtre_fonction")), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(W("combo_filtre_statut")), 0);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(W("chk_licence")), FALSE);
    pret = 1;
    afficher_liste();
}

void on_rb_tri_asc_toggled(GtkWidget *w, gpointer data)
{
    (void)data;
    if (!pret || !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) return;
    tri_croissant = 1;
    afficher_liste();
}

void on_rb_tri_desc_toggled(GtkWidget *w, gpointer data)
{
    (void)data;
    if (!pret || !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w))) return;
    tri_croissant = 0;
    afficher_liste();
}

void on_combo_filtre_fonction_changed(GtkWidget *w, gpointer data)
{ (void)w; (void)data; if (pret) afficher_liste(); }

void on_combo_filtre_statut_changed(GtkWidget *w, gpointer data)
{ (void)w; (void)data; if (pret) afficher_liste(); }

void on_chk_licence_toggled(GtkWidget *w, gpointer data)
{ (void)w; (void)data; if (pret) afficher_liste(); }

void on_btn_menu_clicked(GtkWidget *w, gpointer data)
{
    (void)w; (void)data;
    gtk_widget_destroy(W("window_gestion"));
    ouvrir_principal();
}

void on_btn_deconnexion_gest_clicked(GtkWidget *w, gpointer data)
{
    (void)w; (void)data;
    gtk_widget_destroy(W("window_gestion"));
    ouvrir_login();
}
