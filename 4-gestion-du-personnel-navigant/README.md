# Gestion du personnel navigant - Carthage Airways

Projet de programmation C  - interface **Glade 2 / GTK 2** (libglade2), Ubuntu 18.04.


## Les deux espaces

Une seule fenêtre de connexion, puis l'application ouvre l'espace correspondant au compte .

| Espace | Compte de test | Ce qu'on y fait |
|---|---|---|
| **Administrateur** | `admin` / `admin123` | Menu principal, puis **Gestion du personnel navigant** (ajouter, modifier, supprimer, rechercher, trier, filtrer, statistiques) |
| **Personnel navigant** | `PN001` / `pilote123` | Consulter son **profil** et la validité de sa licence, modifier ses **coordonnées**, consulter les **avions** |

Autres comptes du personnel navigant : `PN002` / `copilote123`, `PN003` / `hotesse123`, `PN004` / `steward123`.
Le matricule saisi comme identifiant doit exister dans `personnel.txt`.

### Espace administrateur
- fiche *Informations du membre* : matricule, nom, prénom, CIN, fonction (Pilote, Copilote, Hôtesse, Steward), base d'attache, date d'embauche, heures de vol, n° de licence, expiration de la licence, téléphone, email, statut (Disponible, En vol, En congé) ;
- boutons **Ajouter**, **Modifier**, **Supprimer**, **Vider le formulaire** ;
- **Liste du personnel** (GtkTreeView) : recherche (tous les champs, matricule, nom, prénom, base), tri A->Z / Z->A sur le nom, filtres *Fonction* et *Statut*, case « Licences à renouveler uniquement » (licence expirée ou qui expire dans 90 jours), compteur « N membres affichés » ;
- clic sur une ligne : la fiche se remplit ;
- onglet *Statistiques* : nombre de membres par fonction et par statut, licences à renouveler.

### Espace personnel navigant
- onglet *Mon profil* : informations personnelles et message sur la validité de la licence (valide / à renouveler bientôt / expirée) ;
- onglet *Mes coordonnées* : modification du téléphone et de l'email (contrôlés, puis enregistrés dans `personnel.txt`) ;
- onglet *Avions* : consultation en lecture seule des avions (recherche, liste, détails de l'avion), à partir de `avions.txt`.

Toutes les données sont dans des fichiers texte : on voit le résultat directement dans le fichier après chaque opération.

## Respect des instructions du devoir

| Instruction | Où |
|---|---|
| Implémenter les fonctions demandées | `personnel.c` / `personnel.h` |
| Travailler avec des fichiers texte | `personnel.txt` (une ligne par membre, champs séparés par `;`) |
| Nommer le module d'après la structure | structure `Personnel` -> `personnel.h` + `personnel.c` |
| Jamais de `printf` / `scanf` dans les fonctions | aucune dans `personnel.c` ; l'affichage se fait dans l'interface (GtkTreeView) |
| Vérifier l'exécution directement dans le fichier texte | `make test` écrit `test_personnel.txt`, `tri_personnel.txt` et `rapport_personnel.txt` |
| Glade 2 uniquement | `gestion_personnel.glade` (4 fenêtres : login, menu principal, gestion admin, espace personnel) |

## Compiler et lancer

```bash
sudo apt install build-essential libgtk2.0-dev libglade2-dev    
cd gestion_personnel
make                    
./gestion_personnel     
make test                
make clean               
```

Pour modifier l'interface avec Glade 2 : `glade-2 gestion_personnel.glade`


## Les fichiers

| Fichier | Rôle |
|---|---|
| `date.h` | structure `Date` (jour, mois, année) |
| `personnel.h` / `personnel.c` | **module Personnel** : fichier texte, recherche, tri, contrôle de saisie (aucun affichage) |
| `test_personnel.c` | test du module sans interface (48 vérifications) |
| `avion.h` / `avion.c` | module Avion (copie de la gestion des avions, utilisé en lecture seule par l'espace personnel) |
| `main.c` | point d'entrée : lance GTK et la fenêtre de connexion |
| `login.c` | connexion : lit `users.txt` et ouvre l'espace administrateur ou l'espace personnel |
| `principal.c` | menu principal de l'administrateur |
| `gestion_personnel.c` | fenêtre *Gestion du personnel navigant* (administrateur) |
| `espace.c` | espace du personnel navigant (profil, coordonnées, avions) |
| `outils.c` | petites fonctions d'aide GTK (listes déroulantes, GtkTreeView, texte) |
| `app.h` | déclarations communes de l'interface |
| `gestion_personnel.glade` | interface Glade 2 |
| `personnel.txt` | données : 6 membres du personnel navigant |
| `avions.txt` | données : 5 avions (consultation) |
| `users.txt` | comptes : `identifiant;motdepasse;role` (role = `admin` ou `personnel`) |
| `Makefile` | `make`, `make test`, `make clean` |

## Le module Personnel (`personnel.h`)

```c
typedef struct { int jour, mois, annee; } Date;          /* date.h */
typedef struct {
    char matricule[12], nom[30], prenom[30], cin[9], fonction[16], base[24];
    Date embauche; int heures; char licence[16]; Date expiration;
    char tel[9], email[48], statut[16];
} Personnel;
```

| Fonction | Rôle | Valeur retournée |
|---|---|---|
| `ajouter_personnel(fichier, p)` | ajoute une ligne à la fin du fichier | 1 ok / 0 matricule déjà utilisé / -2 CIN déjà utilisé / -1 erreur fichier |
| `modifier_personnel(fichier, matricule, p)` | remplace la ligne de ce membre | 1 ok / 0 introuvable / -2 CIN pris par un autre / -1 erreur fichier |
| `supprimer_personnel(fichier, matricule)` | supprime la ligne de ce membre | 1 ok / 0 introuvable / -1 erreur fichier |
| `chercher_personnel(fichier, matricule)` | retourne le membre trouvé | `matricule` vide si introuvable |
| `modifier_coordonnees(fichier, matricule, tel, email)` | change seulement le téléphone et l'email (espace personnel) | 1 ok / 0 introuvable / -1 erreur |
| `charger_personnel(fichier, tab, max)` | lit tout le fichier dans un tableau | nombre lu / -1 si fichier absent |
| `sauvegarder_personnel(fichier, tab, n)` | réécrit tout le fichier | 1 ok / -1 erreur |
| `filtrer_personnel(tab, n, res, texte, critere, fonction, statut)` | recherche + filtres | nombre de résultats |
| `trier_personnel(tab, n, croissant)` | tri par nom | - |
| `compter_fonction(tab, n, fonction)` / `compter_statut(tab, n, statut)` | statistiques | entier |
| `jours_restants(date)` | jours entre aujourd'hui et la date (négatif si passée) | entier |
| `valider_personnel(p)` / `message_erreur_personnel(code)` | contrôle de saisie | 0 si valide, sinon numéro de l'erreur |

Format d'une ligne de `personnel.txt` :

```
matricule;nom;prenom;cin;fonction;base;jj/mm/aaaa;heures;licence;jj/mm/aaaa;tel;email;statut
PN001;Ben Salah;Karim;09123456;Pilote;Tunis-Carthage;15/03/2010;8450;LIC-P-1001;20/03/2027;98123456;karim@carthage.tn;Disponible
```

## Contrôles de saisie

Matricule de 3 caractères minimum (lettres, chiffres, tiret), nom et prénom en lettres, CIN de 8 chiffres, fonction choisie, licence de 3 caractères minimum, dates valides (ex. 31/11 refusé), téléphone de 8 chiffres, email valide, statut choisi. Le matricule et le CIN doivent être uniques ; le matricule ne se modifie plus une fois le membre créé.

## Créer un compte pour un nouveau membre

Après avoir ajouté un membre dans la gestion, ajouter une ligne dans `users.txt` : `matricule;motdepasse;personnel` (par exemple `PN007;monmotdepasse;personnel`).

 thanks for being here :)
