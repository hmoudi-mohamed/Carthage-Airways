#include "aeroport.h"
#include <string.h>

int ajouter(char * filename, aeroport p )
{
    FILE * f=fopen(filename, "a");
    if(f!=NULL)
    {
        fprintf(f,"%d %s %s %s %s %d %d %d\n",p.id,p.code_iata,p.nom,p.ville,p.pays,p.ouvert,p.international,p.capacite);
        fclose(f);
        return 1;
    }
    else return 0;
}
int modifier( char * filename, int id, aeroport nouv )
{
    int tr=0;
    aeroport p;
    FILE * f=fopen(filename, "r");
    FILE * f2=fopen("nouv.txt", "w");
    if(f!=NULL && f2!=NULL)
    {
        while(fscanf(f,"%d %s %s %s %s %d %d %d\n",&p.id,p.code_iata,p.nom,p.ville,p.pays,&p.ouvert,&p.international,&p.capacite)!=EOF)
        {
            if(p.id== id)
            {
                fprintf(f2,"%d %s %s %s %s %d %d %d\n",nouv.id,nouv.code_iata,nouv.nom,nouv.ville,nouv.pays,nouv.ouvert,nouv.international,nouv.capacite);
                tr=1;
            }
            else
                fprintf(f2,"%d %s %s %s %s %d %d %d\n",p.id,p.code_iata,p.nom,p.ville,p.pays,p.ouvert,p.international,p.capacite);

        }
        fclose(f);
        fclose(f2);
        remove(filename);
        rename("nouv.txt", filename);
    }
    return tr;

}
int supprimer(char * filename, int id)
{
    int tr=0;
    aeroport p;
    FILE * f=fopen(filename, "r");
    FILE * f2=fopen("nouv.txt", "w");
    if(f!=NULL && f2!=NULL)
    {
        while(fscanf(f,"%d %s %s %s %s %d %d %d\n",&p.id,p.code_iata,p.nom,p.ville,p.pays,&p.ouvert,&p.international,&p.capacite)!=EOF)
        {
            if(p.id== id)
                tr=1;
            else
                fprintf(f2,"%d %s %s %s %s %d %d %d\n",p.id,p.code_iata,p.nom,p.ville,p.pays,p.ouvert,p.international,p.capacite);
        }
        fclose(f);
        fclose(f2);
        remove(filename);
        rename("nouv.txt", filename);
    }
    return tr;
}
aeroport chercher(char * filename, int id)
{
    aeroport p;
    int tr=0;
    FILE * f=fopen(filename, "r");
    if(f!=NULL)
    {
        while(tr==0&& fscanf(f,"%d %s %s %s %s %d %d %d\n",&p.id,p.code_iata,p.nom,p.ville,p.pays,&p.ouvert,&p.international,&p.capacite)!=EOF)
        {
            if(p.id== id)
                tr=1;
        }
        fclose(f);
    }
    if(tr==0)
        p.id=-1;
    return p;

}

/* ================= Fonctions avancees ================= */

int texte_valide(char * s)
{
    int i;
    if(strlen(s)==0)
        return 0;
    for(i=0;s[i]!='\0';i++)
        if(s[i]==' ' || s[i]=='\t' || s[i]=='\n')
            return 0;
    return 1;
}
int controle_saisie(aeroport a)
{
    int i;
    if(a.id<=0)
        return -1;
    if(strlen(a.code_iata)!=3)
        return -2;
    for(i=0;i<3;i++)
        if(a.code_iata[i]<'A' || a.code_iata[i]>'Z')
            return -2;
    if(texte_valide(a.nom)==0)
        return -3;
    if(texte_valide(a.ville)==0)
        return -4;
    if(texte_valide(a.pays)==0)
        return -5;
    if(a.ouvert!=0 && a.ouvert!=1)
        return -6;
    if(a.international!=0 && a.international!=1)
        return -7;
    if(a.capacite<1 || a.capacite>500)
        return -8;
    return 1;
}
aeroport chercher_code(char * filename, char * code)
{
    aeroport p;
    int tr=0;
    FILE * f=fopen(filename, "r");
    if(f!=NULL)
    {
        while(tr==0&& fscanf(f,"%d %s %s %s %s %d %d %d\n",&p.id,p.code_iata,p.nom,p.ville,p.pays,&p.ouvert,&p.international,&p.capacite)==8)
        {
            if(strcmp(p.code_iata,code)==0)
                tr=1;
        }
        fclose(f);
    }
    if(tr==0)
        p.id=-1;
    return p;
}
int ajouter_controle(char * filename, aeroport a)
{
    int c=controle_saisie(a);
    if(c!=1)
        return c;
    if(chercher(filename,a.id).id!=-1)
        return -9;
    if(chercher_code(filename,a.code_iata).id!=-1)
        return -10;
    return ajouter(filename,a);
}
int modifier_controle(char * filename, int id, aeroport nouv)
{
    aeroport p;
    int c=controle_saisie(nouv);
    if(c!=1)
        return c;
    if(chercher(filename,id).id==-1)
        return 0;
    if(nouv.id!=id && chercher(filename,nouv.id).id!=-1)
        return -9;
    p=chercher_code(filename,nouv.code_iata);
    if(p.id!=-1 && p.id!=id)
        return -10;
    return modifier(filename,id,nouv);
}
int rechercher(char * filename, char * resultat, int critere, char * valeur)
{
    int n=0,ok;
    aeroport p;
    FILE * f=fopen(filename, "r");
    FILE * f2=fopen(resultat, "w");
    if(f==NULL || f2==NULL)
    {
        if(f!=NULL)
            fclose(f);
        if(f2!=NULL)
            fclose(f2);
        return -1;
    }
    while(fscanf(f,"%d %s %s %s %s %d %d %d\n",&p.id,p.code_iata,p.nom,p.ville,p.pays,&p.ouvert,&p.international,&p.capacite)==8)
    {
        ok=0;
        if(critere==1 && strcmp(p.code_iata,valeur)==0)
            ok=1;
        if(critere==2 && strcmp(p.ville,valeur)==0)
            ok=1;
        if(critere==3 && strcmp(p.pays,valeur)==0)
            ok=1;
        if(critere==4 && strcmp(p.nom,valeur)==0)
            ok=1;
        if(ok==1)
        {
            fprintf(f2,"%d %s %s %s %s %d %d %d\n",p.id,p.code_iata,p.nom,p.ville,p.pays,p.ouvert,p.international,p.capacite);
            n++;
        }
    }
    fclose(f);
    fclose(f2);
    return n;
}
int filtrer(char * filename, char * resultat, char * pays, int etat, int international)
{
    int n=0;
    aeroport p;
    FILE * f=fopen(filename, "r");
    FILE * f2=fopen(resultat, "w");
    if(f==NULL || f2==NULL)
    {
        if(f!=NULL)
            fclose(f);
        if(f2!=NULL)
            fclose(f2);
        return -1;
    }
    while(fscanf(f,"%d %s %s %s %s %d %d %d\n",&p.id,p.code_iata,p.nom,p.ville,p.pays,&p.ouvert,&p.international,&p.capacite)==8)
    {
        if((strcmp(pays,"*")==0 || strcmp(p.pays,pays)==0) && (etat==-1 || p.ouvert==etat) && (international==-1 || p.international==international))
        {
            fprintf(f2,"%d %s %s %s %s %d %d %d\n",p.id,p.code_iata,p.nom,p.ville,p.pays,p.ouvert,p.international,p.capacite);
            n++;
        }
    }
    fclose(f);
    fclose(f2);
    return n;
}
int trier(char * filename, char * resultat, int ordre)
{
    aeroport tab[MAX_AEROPORTS],tmp;
    int n=0,i,j;
    FILE * f=fopen(filename, "r");
    FILE * f2;
    if(f==NULL)
        return -1;
    while(n<MAX_AEROPORTS && fscanf(f,"%d %s %s %s %s %d %d %d\n",&tab[n].id,tab[n].code_iata,tab[n].nom,tab[n].ville,tab[n].pays,&tab[n].ouvert,&tab[n].international,&tab[n].capacite)==8)
        n++;
    fclose(f);
    for(i=0;i<n-1;i++)
        for(j=0;j<n-1-i;j++)
            if((ordre==1 && strcmp(tab[j].nom,tab[j+1].nom)>0) || (ordre==2 && strcmp(tab[j].nom,tab[j+1].nom)<0))
            {
                tmp=tab[j];
                tab[j]=tab[j+1];
                tab[j+1]=tmp;
            }
    f2=fopen(resultat, "w");
    if(f2==NULL)
        return -1;
    for(i=0;i<n;i++)
        fprintf(f2,"%d %s %s %s %s %d %d %d\n",tab[i].id,tab[i].code_iata,tab[i].nom,tab[i].ville,tab[i].pays,tab[i].ouvert,tab[i].international,tab[i].capacite);
    fclose(f2);
    return n;
}
