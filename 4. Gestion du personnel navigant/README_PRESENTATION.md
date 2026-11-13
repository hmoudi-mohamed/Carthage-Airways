# Présentation séance 4 - Gestion du personnel navigant

**Module : `personnel.h / personnel.c` (nom de la structure `Personnel`)** + `historique.h / .c`.

1. **Données** : `personnel.txt`, une ligne = un enregistrement, champs séparés par `;`.
2. **Logique** (aucun affichage) : ajouter, modifier, supprimer, chercher, filtrer, trier, compter, exporter, valider -> chaque fonction retourne un code.
3. **Vérification dans le fichier** : `make test` écrit `test_personnel.txt`, `tri_personnel.txt`, `export_personnel.csv`, `rapport_personnel.txt` (à ouvrir).
4. **Deux interfaces Glade 2** : *administrateur* (gestion, statistiques, historique) et *personnel navigant* (consultation + modification de ses coordonnées).
5. **Démonstration conseillée** : se connecter en `admin`, ajouter / modifier / supprimer, regarder le fichier `personnel.txt` et `historique.txt` ; se déconnecter ; se connecter en `PN001` / `pilote123`.
