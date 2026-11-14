# Gestion du personnel navigant - Carthage Airways

Projet de programmation C  - interface **Glade 2 / GTK 2** (libglade2), Ubuntu 18.04.


 Les fichiers

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




 thanks for being here :)
