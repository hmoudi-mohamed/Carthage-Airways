#include <stdio.h>
#include <string.h>
#include "avion.h"
static void vider(void)
{
    int c;	
    while ((c=getchar()) != '\n'&& c!=EOF);
}

static int lire_entier(char *msg)
{	
   int n;
   printf("%s",msg);
   while (scanf("%d", &n) !=1)
   {
     vider();
	printf("Entrer un nombre:");
}
	return n;
}
static avion saisir_avion(void)
{
	avion a ;
	printf("Immatriculcation (exp: TS-ABC):"); scanf("%19s",a.immatriculation);
	printf("Modele (exp:A320):"); scanf("%29s",a.modele);
	printf("Constructeur (exp: Airbus):"); scanf("%29s", a.constructeur);
	a.type= lire_entier("Type(0:long, 1:Moyen, 2:long, 3:Cargo  ...)"); 
	printf("Base (exp: Tunis, - si vide):"); scanf("%29s", a.base);
	if (strcmp(a.base,"-")==0)
	  a.base[0]='\0';
       	a.jour=lire_entier("Jour:");
	a.mois=lire_entier("mois:");
	a.annee=lire_entier("annee:");
	a.capacite_max= lire_entier("Capacite max:");
  	a.etat=lire_entier("Etat: (0:opérationnel, 1:En maintenace, 2:Hors service):");
	return a;
}
static void afficher_avion(avion a)
{
printf("Immatriculation:%s\n",a.immatriculation);
printf("Modele:%s\n",a.modele);
printf("constructeur:%s\n",a.constructeur);
printf("type:%d\n",a.type);
printf("Base:%s\n",strlen(a.base)?a.base:"(vide)");
printf("Date:%d/%d/%d\n",a.jour,a.mois,a.annee);
printf("Capacite max:%d\n",a.capacite_max);
printf("Etat:%d\n",a.etat);
}
int main()
{
int x;
char immat[20];
avion a;

int choix;
printf("\n-----MENU-----\n");
printf("1.ajout d'un avion\n");
printf("2.modification d'une immatriculation d'un avion\n");
printf("3.Suppression d'un avion\n");
printf("4.Chercher un avion\n");
printf("5.Quitter\n");
do
{
printf("Donner votre choix:");
scanf(" %d",&choix);
}
while ((choix<1)||(choix>5));
switch(choix)
{
case 1:
	a= saisir_avion();
 	x=ajouter("avion.txt", a);
	if(x==1)
        printf("\najout d'avion avec succés\n");
    	else if (x==-1) printf("\ncette immatriculation existe deja\n");
    	else printf("\nechec ajout!\n");
	break;
case 2:
	printf("Immatriculation a modifier:");
	scanf("%19s", immat);
	printf("Saisiiez les nouvelles valeurs:\n");
	a= saisir_avion();
	x=modifier("avion.txt", immat,a);
    	if(x==1)
        printf("\nModification d'avion avec succés\n");
	else if (x==0) printf("\nechec Modification!\n");
    	else printf("\nla nouvelle immatriculation existe deja!\n");
	break;
case 3:
	printf("Immatriculation a supprimer:");
	scanf("%19s",immat);
	x=supprimer("avion.txt",immat);
    	if(x==1)
        printf("\nSuppression d'avion avec succés\n");
    	else printf("\nechec Suppression\n");
	break;
case 4:
	printf("Immatriculation a chercher:");
	scanf("%19s",immat);
	a=chercher("avion.txt",immat );
    	if(a.capacite_max==-1)
        printf("introuvable!\n");
    	else
	afficher_avion(a);
    	break;
case 5:
	printf("A la prochaine!\n");
	break;
}
return 0;
}
