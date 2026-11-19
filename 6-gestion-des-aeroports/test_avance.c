#include <stdio.h>
#include <string.h>
#include "aeroport.h"

void afficher(char * action, int x)
{
    printf("\n%s : ", action);
    if(x==1) printf("succes");
    else if(x==0) printf("echec (fichier ou aeroport introuvable)");
    else if(x==-1) printf("echec (id invalide)");
    else if(x==-2) printf("echec (code IATA invalide)");
    else if(x==-3) printf("echec (nom invalide)");
    else if(x==-4) printf("echec (ville invalide)");
    else if(x==-5) printf("echec (pays invalide)");
    else if(x==-6) printf("echec (etat ouvert invalide)");
    else if(x==-7) printf("echec (international invalide)");
    else if(x==-8) printf("echec (capacite invalide)");
    else if(x==-9) printf("echec (id deja existant)");
    else if(x==-10) printf("echec (code IATA deja existant)");
}

int main()
{
    char * fic="aeroport.txt";
    aeroport a1= {"TUN","Tunis-Carthage","Tunis","Tunisie",1,1,1,120};
    aeroport a2= {"DJE","Djerba-Zarzis","Djerba","Tunisie",2,1,1,60};
    aeroport a3= {"MIR","Monastir-Habib-Bourguiba","Monastir","Tunisie",3,1,1,80};
    aeroport a4= {"CDG","Paris-Charles-de-Gaulle","Paris","France",4,0,1,300};
    aeroport doublon_id= {"XXX","Autre","Autre","Tunisie",1,1,0,10};
    aeroport doublon_code= {"TUN","Autre","Autre","Tunisie",9,1,0,10};
    aeroport mauvais_code= {"tun","Autre","Autre","Tunisie",5,1,0,10};
    aeroport mauvais_nom= {"ABC","Nom avec espace","Autre","Tunisie",5,1,0,10};
    aeroport mauvaise_cap= {"ABC","Autre","Autre","Tunisie",5,1,0,600};
    aeroport a2bis= {"DJE","Djerba-Zarzis","Djerba","Tunisie",2,0,1,90};
    aeroport code_pris= {"TUN","Djerba-Zarzis","Djerba","Tunisie",2,0,1,90};
    aeroport id_pris= {"DJE","Djerba-Zarzis","Djerba","Tunisie",1,0,1,90};
    aeroport p;
    int n;

    remove(fic);
    afficher("Ajout 1 TUN",ajouter_controle(fic,a1));
    afficher("Ajout 2 DJE",ajouter_controle(fic,a2));
    afficher("Ajout 3 MIR",ajouter_controle(fic,a3));
    afficher("Ajout 4 CDG",ajouter_controle(fic,a4));
    afficher("Ajout 1 TUN (deja existant)",ajouter_controle(fic,a1));
    afficher("Ajout meme id 1",ajouter_controle(fic,doublon_id));
    afficher("Ajout meme code TUN",ajouter_controle(fic,doublon_code));
    afficher("Ajout code 'tun'",ajouter_controle(fic,mauvais_code));
    afficher("Ajout nom avec espace",ajouter_controle(fic,mauvais_nom));
    afficher("Ajout capacite 600",ajouter_controle(fic,mauvaise_cap));

    afficher("Modification 2 (DJE ferme)",modifier_controle(fic,2,a2bis));
    afficher("Modification 2 avec code TUN",modifier_controle(fic,2,code_pris));
    afficher("Modification 2 avec id 1",modifier_controle(fic,2,id_pris));
    afficher("Modification 99 (introuvable)",modifier_controle(fic,99,a2bis));

    p=chercher_code(fic,"CDG");
    if(p.id==-1) printf("\nCDG introuvable");
    else printf("\nCDG trouve : id %d, %s",p.id,p.ville);

    n=rechercher(fic,"res_ville.txt",2,"Tunis");
    printf("\nRecherche ville Tunis : %d resultat(s)",n);
    n=rechercher(fic,"res_code.txt",1,"CDG");
    printf("\nRecherche code CDG : %d resultat(s)",n);
    n=filtrer(fic,"res_filtre1.txt","Tunisie",-1,-1);
    printf("\nFiltre pays Tunisie : %d resultat(s)",n);
    n=filtrer(fic,"res_filtre2.txt","*",0,-1);
    printf("\nFiltre aeroports fermes : %d resultat(s)",n);
    n=trier(fic,"tri_az.txt",1);
    printf("\nTri nom A->Z : %d aeroports",n);
    n=trier(fic,"tri_za.txt",2);
    printf("\nTri nom Z->A : %d aeroports\n",n);
    return 0;
}
