/* espace.c - Espace du personnel navigant : profil, coordonnées et consultation des avions.
 * Le membre connecté est identifié par la variable « utilisateur » (matricule). */
#include <stdio.h>
#include <string.h>
#include "app.h"

static GladeXML *xml_esp;
#define W(id) glade_xml_get_widget(xml_esp, id)

static Personnel moi;                       /* fiche du membre connecté */
static Avion tab_av[MAX_AVIONS], vue_av[MAX_AVIONS];
static int nb_av = 0, nv_av = 0;
static GtkListStore *liste_av;

static const char *titres_av[] = { "Immatriculation", "Modèle", "Constructeur", "Type", "Base d'attache", "Capacité", "État" };

static void texte(const char *id, const char *t) { gtk_label_set_text(GTK_LABEL(W(id)), t); }

/* ------------------------------------------------------------------ */
/*  Profil                                                             */
/* ------------------------------------------------------------------ */
static void afficher_profil(void)
{
    char b[80];
    int j;

    snprintf(b, sizeof b, "%s %s", moi.prenom, moi.nom);
    texte("lbl_p_nom", b);
    texte("lbl_p_matricule", moi.matricule);
    texte("lbl_p_cin", moi.cin);
    texte("lbl_p_fonction", moi.fonction);
    texte("lbl_p_base", moi.base);
    texte("lbl_p_statut", moi.statut);
    snprintf(b, sizeof b, "%02d/%02d/%04d", moi.embauche.jour, moi.embauche.mois, moi.embauche.annee);
    texte("lbl_p_embauche", b);
    snprintf(b, sizeof b, "%d h", moi.heures);
    texte("lbl_p_heures", b);
    texte("lbl_p_licence", moi.licence);
    snprintf(b, sizeof b, "%02d/%02d/%04d", moi.expiration.jour, moi.expiration.mois, moi.expiration.annee);
    texte("lbl_p_expiration", b);

    j = jours_restants(moi.expiration);
    if (j < 0)        snprintf(b, sizeof b, "Licence expirée depuis %d jour(s) : à renouveler", -j);
    else if (j <= 90) snprintf(b, sizeof b, "Licence à renouveler bientôt : %d jour(s) restants", j);
    else              snprintf(b, sizeof b, "Licence valide : %d jour(s) restants", j);
    texte("lbl_p_validite", b);
}

/* ------------------------------------------------------------------ */
/*  Coordonnées                                                        */
/* ------------------------------------------------------------------ */
static void afficher_coordonnees(void)
{
    ecrire_entry(W("e_tel"), moi.tel);
    ecrire_entry(W("e_email"), moi.email);
}

void on_btn_enregistrer_coord_clicked(GtkWidget *w, gpointer data)
{
    Personnel test;
    char tel[12], email[52];
    int erreur, r;
    (void)w; (void)data;

    copier_texte(tel, sizeof tel, gtk_entry_get_text(GTK_ENTRY(W("e_tel"))));
    copier_texte(email, sizeof email, gtk_entry_get_text(GTK_ENTRY(W("e_email"))));
    enlever_espaces(tel);
    enlever_espaces(email);

    test = moi;                                   /* on contrôle la saisie avec la même fonction que l'administrateur */
    copier_texte(test.tel, sizeof test.tel, tel);
    copier_texte(test.email, sizeof test.email, email);
    erreur = valider_personnel(test);
    if (erreur == 9 || erreur == 10) {
        texte("lbl_msg_coord", message_erreur_personnel(erreur));
        return;
    }
    r = modifier_coordonnees(FICHIER_PERSONNEL, moi.matricule, tel, email);
    if (r == 1) {
        copier_texte(moi.tel, sizeof moi.tel, tel);
        copier_texte(moi.email, sizeof moi.email, email);
        texte("lbl_msg_coord", "Coordonnées enregistrées dans le fichier personnel.txt.");
    } else {
        texte("lbl_msg_coord", "Erreur d'enregistrement.");
    }
}

void on_btn_annuler_coord_clicked(GtkWidget *w, gpointer data)
{
    (void)w; (void)data;
    afficher_coordonnees();
    texte("lbl_msg_coord", "");
}

/* ------------------------------------------------------------------ */
/*  Consultation des avions (lecture seule)                            */
/* ------------------------------------------------------------------ */
static void afficher_avions(void)
{
    int i;
    char texte_rech[64], info[64], cap[16];

    copier_texte(texte_rech, sizeof texte_rech, gtk_entry_get_text(GTK_ENTRY(W("e_recherche_avion"))));
    enlever_espaces(texte_rech);
    nv_av = filtrer_avions(tab_av, nb_av, vue_av, texte_rech, 0, "", "");
    trier_avions(vue_av, nv_av, 1);

    gtk_list_store_clear(liste_av);
    for (i = 0; i < nv_av; i++) {
        GtkTreeIter it;
        snprintf(cap, sizeof cap, "%d", vue_av[i].capacite);
        gtk_list_store_append(liste_av, &it);
        gtk_list_store_set(liste_av, &it, 0, vue_av[i].immat, 1, vue_av[i].modele, 2, vue_av[i].constructeur,
                           3, vue_av[i].type, 4, vue_av[i].base, 5, cap, 6, vue_av[i].etat, -1);
    }
    snprintf(info, sizeof info, "%d avions affichés", nv_av);
    texte("lbl_e_nb", info);
}

void on_btn_rech_avion_clicked(GtkWidget *w, gpointer data)
{ (void)w; (void)data; afficher_avions(); }

void on_e_recherche_avion_activate(GtkWidget *w, gpointer data)
{ on_btn_rech_avion_clicked(w, data); }

void on_btn_reinit_avion_clicked(GtkWidget *w, gpointer data)
{
    (void)w; (void)data;
    ecrire_entry(W("e_recherche_avion"), "");
    afficher_avions();
}

void on_treeview_avions_espace_cursor_changed(GtkWidget *tv, gpointer data)
{
    char immat[12], b[40], date[16];
    Avion a;
    (void)data;

    if (!valeur_selectionnee(tv, 0, immat, sizeof immat)) return;
    a = chercher_avion(FICHIER_AVIONS, immat);
    if (a.immat[0] == '\0') return;
    texte("lbl_d_immat", a.immat);
    texte("lbl_d_modele", a.modele);
    texte("lbl_d_constructeur", a.constructeur);
    texte("lbl_d_type", a.type);
    texte("lbl_d_base", a.base);
    snprintf(date, sizeof date, "%02d/%02d/%04d", a.service.jour, a.service.mois, a.service.annee);
    texte("lbl_d_service", date);
    snprintf(b, sizeof b, "%d", a.capacite);              texte("lbl_d_capacite", b);
    snprintf(b, sizeof b, "%d", a.reservees);             texte("lbl_d_reservees", b);
    snprintf(b, sizeof b, "%d", places_disponibles(a));   texte("lbl_d_places", b);
    texte("lbl_d_etat", a.etat);
}

/* ------------------------------------------------------------------ */
/*  Ouverture de la fenêtre                                            */
/* ------------------------------------------------------------------ */
void ouvrir_espace(void)
{
    GtkWidget *fenetre;
    char b[100];

    moi = chercher_personnel(FICHIER_PERSONNEL, utilisateur);
    xml_esp = glade_xml_new(FICHIER_GLADE, "window_espace", NULL);
    glade_xml_signal_autoconnect(xml_esp);
    fenetre = W("window_espace");
    g_signal_connect(fenetre, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

    snprintf(b, sizeof b, "Connecté : %s %s (%s)", moi.prenom, moi.nom, moi.fonction);
    texte("lbl_user_esp", b);

    liste_av = preparer_liste(W("treeview_avions_espace"), 7, titres_av);
    nb_av = charger_avions(FICHIER_AVIONS, tab_av, MAX_AVIONS);
    if (nb_av < 0) nb_av = 0;

    afficher_profil();
    afficher_coordonnees();
    afficher_avions();
    afficher_statut(W("statusbar_esp"), "Espace personnel navigant");
    gtk_widget_show_all(fenetre);
}

void on_btn_deconnexion_esp_clicked(GtkWidget *w, gpointer data)
{
    (void)w; (void)data;
    gtk_widget_destroy(W("window_espace"));
    ouvrir_login();
}
