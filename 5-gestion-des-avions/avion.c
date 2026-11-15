#include "avion.h"
#include <string.h>

int ajouter(char * filename, avion a )
{
    avion test;
    FILE * f;
    test=chercher(filename, a.immatriculation);
    if(test.capacite_max!=-1)
        return -1;
   if(strlen(a.base)==0)
     strcpy(a.base,"-");
    f=fopen(filename, "a");
    if(f!=NULL)
    {
        fprintf(f,"%s %s %s %d %s %d %d %d %d %d\n",a.immatriculation,a.modele,a.constructeur,a.type,a.base,a.jour,a.mois,a.annee,a.capacite_max,a.etat);
        fclose(f);
        return 1;
    }
    else return 0;
}


int modifier(char*filename, char * immat, avion nouv )
{
int tr=0;
    avion a, test;
    FILE * f;
    FILE * f2;
    test=chercher(filename, immat);
    if(test.capacite_max==-1)
        return 0;                       
    if(strcmp(immat,nouv.immatriculation)!=0)
    {
        test=chercher(filename, nouv.immatriculation);
        if(test.capacite_max!=-1)
            return -1;                  
    }
    if(strlen(nouv.base)==0)
        strcpy(nouv.base,"-");
    f=fopen(filename, "r");
    f2=fopen("nouv.txt", "w");
    if(f!=NULL && f2!=NULL)
    {
        while(fscanf(f,"%19s %29s %29s %d %29s %d %d %d %d %d\n",a.immatriculation,a.modele,a.constructeur,&a.type,a.base,&a.jour,&a.mois,&a.annee,&a.capacite_max,&a.etat)==10)
        {
     if(strcmp(a.immatriculation,immat)==0)
     {fprintf(f2,"%s %s %s %d %s %d %d %d %d %d\n",nouv.immatriculation,nouv.modele,nouv.constructeur,nouv.type,nouv.base,nouv.jour,nouv.mois,nouv.annee,nouv.capacite_max,nouv.etat);
                tr=1;
            }
            else
                fprintf(f2,"%s %s %s %d %s %d %d %d %d %d\n",a.immatriculation,a.modele,a.constructeur,a.type,a.base,a.jour,a.mois,a.annee,a.capacite_max,a.etat);
        }
        fclose(f);
        fclose(f2);
        remove(filename);
        rename("nouv.txt", filename);
    }
    else
    {
        if(f!=NULL) fclose(f);
        if(f2!=NULL) fclose(f2);
    }
    return tr;
}
int supprimer(char * filename, char * immat)
{
    int tr=0;
    avion a;
    FILE * f;
    FILE * f2;
    f=fopen(filename, "r");
    f2=fopen("nouv.txt", "w");
    if(f!=NULL && f2!=NULL)
    {
        while(fscanf(f,"%19s %29s %29s %d %29s %d %d %d %d %d\n",a.immatriculation,a.modele,a.constructeur,&a.type,a.base,&a.jour,&a.mois,&a.annee,&a.capacite_max,&a.etat)==10)
        {
            if(strcmp(a.immatriculation,immat)==0)
                tr=1;
            else
                fprintf(f2,"%s %s %s %d %s %d %d %d %d %d\n",a.immatriculation,a.modele,a.constructeur,a.type,a.base,a.jour,a.mois,a.annee,a.capacite_max,a.etat);
        }
        fclose(f);
        fclose(f2);
        remove(filename);
        rename("nouv.txt", filename);
    }
    else
    {
        if(f!=NULL) fclose(f);
        if(f2!=NULL) fclose(f2);
    }
    return tr;
}
avion chercher(char * filename, char * immat)
{
    avion a;
    int tr=0;
    FILE * f=fopen(filename, "r");
    if(f!=NULL)
    {
while(tr==0 &&fscanf(f,"%19s %29s %29s %d %29s %d %d %d %d %d\n",a.immatriculation,a.modele,a.constructeur,&a.type,a.base,&a.jour,&a.mois,&a.annee,&a.capacite_max,&a.etat)==10)

        {
            if(strcmp(a.immatriculation,immat)==0)
                tr=1;
      }
        fclose(f);
    }
    if(tr==0)
    {
        a.immatriculation[0]='\0';
        a.capacite_max=-1;               /* introuvable */
    }
    else if(strcmp(a.base,"-")==0)
        a.base[0]='\0';                  /* base vide */
    return a;
}






