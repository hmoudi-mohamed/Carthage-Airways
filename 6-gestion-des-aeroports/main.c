#include <stdio.h>
#include <string.h>
#include"aeroport.h"

int main()
{
    aeroport a1= {"TUN","Tunis-Carthage","Tunis","Tunisie",1,1,1,120},a2= {"DJE","Djerba-Zarzis","Djerba","Tunisie",2,1,1,60},a3= {"MIR","Monastir-Habib-Bourguiba","Monastir","Tunisie",3,1,1,80},a4= {"TUN","Tunis-Carthage","Tunis","Tunisie",1,0,1,150},p;
    int x=ajouter("aeroport.txt", a1);
    if(x==1)
        printf("\najout de aeroport avec succes");
    else printf("\nechec ajout");
    x=ajouter("aeroport.txt", a2);
    x=ajouter("aeroport.txt", a3);
    x=modifier("aeroport.txt",1,a4 );

    if(x==1)
        printf("\nModification de aeroport avec succes");
    else printf("\nechec Modification");
    x=supprimer("aeroport.txt",2 );
    if(x==1)
        printf("\nSuppression de aeroport avec succes");
    else printf("\nechec Suppression");
    p=chercher("aeroport.txt",5 );
    if(p.id==-1)
        printf("\nintrouvable");
    return 0;
}
