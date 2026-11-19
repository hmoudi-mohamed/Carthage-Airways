#include <stdio.h>
#include <string.h>
#include "aeroport.h"

#define FICHIER "aeroport.txt"

/* ---------- messages d'erreur ---------- */

void texte_erreur(int x)
{
    if(x==0) printf("(fichier ou aeroport introuvable)");
    else if(x==-1) printf("(identifiant invalide, il doit etre > 0)");
    else if(x==-2) printf("(code IATA invalide : exactement 3 lettres majuscules)");
    else if(x==-3) printf("(nom invalide : vide, trop long ou avec espace)");
    else if(x==-4) printf("(ville invalide : vide, trop longue ou avec espace)");
    else if(x==-5) printf("(pays invalide : vide, trop long ou avec espace)");
    else if(x==-6) printf("(la valeur doit etre 1 ou 0)");
    else if(x==-7) printf("(la valeur doit etre 1 ou 0)");
    else if(x==-8) printf("(la capacite doit etre entre 1 et 500)");
    else if(x==-9) printf("(cet identifiant existe deja)");
    else if(x==-10) printf("(ce code IATA existe deja)");
}

void message(char * action, int x)
{
    printf("\n%s : ",action);
    if(x==1)
        printf("succes\n");
    else
    {
        printf("echec ");
        texte_erreur(x);
        printf("\n");
    }
}

/* ---------- lecture au clavier (uniquement dans ce fichier, pas dans le module) ---------- */

void vider_buffer()
{
    int c;
    while((c=getchar())!='\n' && c!=EOF);
}

int lire_entier(char * question)
{
    int x,r;
    printf("%s",question);
    r=scanf("%d",&x);
    while(r!=1)
    {
        if(r==EOF)
            return 0;
        vider_buffer();
        printf("Valeur invalide, entrez un nombre : ");
        r=scanf("%d",&x);
    }
    vider_buffer();
    return x;
}

void lire_texte(char * question, char * dest, int taille)
{
    char tmp[100];
    int c,reste=0;
    printf("%s",question);
    if(scanf("%99s",tmp)!=1)
        tmp[0]='\0';
    c=getchar();
    while(c==' ' || c=='\t')
        c=getchar();
    while(c!='\n' && c!=EOF)  /* des caracteres restent sur la ligne (ex: un espace dans le nom) */
    {
        reste=1;
        c=getchar();
    }
    if(reste==0 && (int)strlen(tmp)<taille)
        strcpy(dest,tmp);
    else
        dest[0]='\0';          /* espace dans le texte ou texte trop long : refuse par le controle */
}

/* ---------- controle immediat de chaque champ ---------- */

/* Controle de saisie + unicite. num = champ qu'on vient de saisir :
   1 = identifiant, 2 = code IATA (les autres champs n'ont pas de test d'unicite).
   ancien_id = id de l'aeroport modifie (-1 pour un ajout), pour ne pas se bloquer sur soi-meme */
int verifier(aeroport a, int ancien_id, int num)
{
    aeroport p;
    int c=controle_saisie(a);
    if(c!=1)
        return c;
    if(num==1)
    {
        p=chercher(FICHIER,a.id);
        if(p.id!=-1 && a.id!=ancien_id)
            return -9;
    }
    if(num==2)
    {
        p=chercher_code(FICHIER,a.code_iata);
        if(p.id!=-1 && p.id!=ancien_id)
            return -10;
    }
    return 1;
}

int saisir_entier(char * question, int * champ, aeroport * a, int ancien_id, int num)
{
    int c;
    do
    {
        *champ=lire_entier(question);
        c=verifier(*a,ancien_id,num);
        if(c!=1)
        {
            printf("  Erreur ");
            texte_erreur(c);
            printf("\n");
        }
    }
    while(c!=1 && !feof(stdin));
    return c==1;
}

int saisir_texte(char * question, char * dest, int taille, aeroport * a, int ancien_id, int num)
{
    int c;
    do
    {
        lire_texte(question,dest,taille);
        c=verifier(*a,ancien_id,num);
        if(c!=1)
        {
            printf("  Erreur ");
            texte_erreur(c);
            printf("\n");
        }
    }
    while(c!=1 && !feof(stdin));
    return c==1;
}

/* Saisit un aeroport champ par champ. Renvoie 1 si termine, 0 si annule (id = 0). */
int lire_aeroport(aeroport * a, int ancien_id)
{
    int c;
    /* valeurs par defaut valides : on peut ainsi controler un seul champ a la fois */
    strcpy(a->code_iata,"TUN");
    strcpy(a->nom,"Nom");
    strcpy(a->ville,"Ville");
    strcpy(a->pays,"Pays");
    a->id=1;
    a->ouvert=1;
    a->international=1;
    a->capacite=100;

    do
    {
        a->id=lire_entier("Identifiant (entier > 0, 0 pour annuler) : ");
        if(a->id==0)
            return 0;
        c=verifier(*a,ancien_id,1);
        if(c!=1)
        {
            printf("  Erreur ");
            texte_erreur(c);
            printf("\n");
        }
    }
    while(c!=1);

    if(!saisir_texte("Code IATA (3 lettres majuscules, ex: TUN) : ",a->code_iata,4,a,ancien_id,2))
        return 0;
    if(!saisir_texte("Nom (sans espace, ex: Tunis-Carthage) : ",a->nom,50,a,ancien_id,3))
        return 0;
    if(!saisir_texte("Ville (sans espace) : ",a->ville,30,a,ancien_id,4))
        return 0;
    if(!saisir_texte("Pays (sans espace) : ",a->pays,30,a,ancien_id,5))
        return 0;
    if(!saisir_entier("Ouvert ? (1 = oui, 0 = non) : ",&a->ouvert,a,ancien_id,6))
        return 0;
    if(!saisir_entier("International ? (1 = oui, 0 = non) : ",&a->international,a,ancien_id,7))
        return 0;
    if(!saisir_entier("Capacite journaliere (1 a 500) : ",&a->capacite,a,ancien_id,8))
        return 0;
    return 1;
}

/* ---------- fonctions d'affichage ---------- */

void afficher_aeroport(aeroport p)
{
    char * etat="ferme";
    char * inter="non";
    if(p.ouvert==1)
        etat="ouvert";
    if(p.international==1)
        inter="oui";
    printf("\nId : %d\nCode IATA : %s\nNom : %s\nVille : %s\nPays : %s\nEtat : %s\nInternational : %s\nCapacite journaliere : %d\n",p.id,p.code_iata,p.nom,p.ville,p.pays,etat,inter,p.capacite);
}

void afficher_liste()
{
    aeroport p;
    int n=0;
    FILE * f=fopen(FICHIER,"r");
    if(f==NULL)
    {
        printf("\nAucun aeroport enregistre (fichier introuvable)\n");
        return;
    }
    printf("\n%-4s %-5s %-26s %-12s %-10s %-7s %-5s %s\n","ID","IATA","Nom","Ville","Pays","Etat","Int.","Cap.");
    while(fscanf(f,"%d %s %s %s %s %d %d %d\n",&p.id,p.code_iata,p.nom,p.ville,p.pays,&p.ouvert,&p.international,&p.capacite)==8)
    {
        printf("%-4d %-5s %-26s %-12s %-10s %-7s %-5s %d\n",p.id,p.code_iata,p.nom,p.ville,p.pays,p.ouvert==1?"ouvert":"ferme",p.international==1?"oui":"non",p.capacite);
        n++;
    }
    fclose(f);
    printf("%d aeroport(s)\n",n);
}

/* ---------- programme principal : menu ---------- */

int main()
{
    int choix,id,x;
    char code[4];
    aeroport a,p;

    do
    {
        printf("\n===== Carthage Airways - Gestion des aeroports =====\n");
        printf("1. Ajouter un aeroport\n");
        printf("2. Modifier un aeroport\n");
        printf("3. Supprimer un aeroport\n");
        printf("4. Chercher un aeroport par identifiant\n");
        printf("5. Chercher un aeroport par code IATA\n");
        printf("6. Afficher la liste des aeroports\n");
        printf("0. Quitter\n");
        choix=lire_entier("Votre choix : ");

        if(choix==1)
        {
            if(lire_aeroport(&a,-1)==1)
            {
                x=ajouter_controle(FICHIER,a);
                message("Ajout",x);
            }
            else
                printf("\nAjout annule\n");
        }
        else if(choix==2)
        {
            id=lire_entier("Identifiant de l'aeroport a modifier : ");
            p=chercher(FICHIER,id);
            if(p.id==-1)
                message("Modification",0);
            else
            {
                printf("\nAeroport actuel :");
                afficher_aeroport(p);
                printf("\nSaisissez les nouvelles informations :\n");
                if(lire_aeroport(&a,id)==1)
                {
                    x=modifier_controle(FICHIER,id,a);
                    message("Modification",x);
                }
                else
                    printf("\nModification annulee\n");
            }
        }
        else if(choix==3)
        {
            id=lire_entier("Identifiant de l'aeroport a supprimer : ");
            x=supprimer(FICHIER,id);
            message("Suppression",x);
        }
        else if(choix==4)
        {
            id=lire_entier("Identifiant a chercher : ");
            p=chercher(FICHIER,id);
            if(p.id==-1)
                printf("\nAeroport introuvable\n");
            else
                afficher_aeroport(p);
        }
        else if(choix==5)
        {
            lire_texte("Code IATA a chercher : ",code,4);
            p=chercher_code(FICHIER,code);
            if(p.id==-1)
                printf("\nAeroport introuvable\n");
            else
                afficher_aeroport(p);
        }
        else if(choix==6)
            afficher_liste();
        else if(choix!=0)
            printf("\nChoix invalide\n");
    }
    while(choix!=0);

    printf("\nAu revoir\n");
    return 0;
}
