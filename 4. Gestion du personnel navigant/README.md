# Gestion du personnel navigant - Carthage Airways

Gestion des pilotes, copilotes, hôtesses et stewards : fonction, base, licence et disponibilité.

Projet de programmation C **(séance 4)** - interface **Glade 2 / GTK 2** (libglade2) - Ubuntu 18.04.
Cette gestion possède **deux interfaces** : l'**espace administrateur** et l'**espace personnel navigant** (utilisateur).

---

## 1. Lancer le projet

```bash
sudo apt install build-essential libgtk2.0-dev libglade2-dev pkg-config   # une seule fois
make                      # compile l'application
./gestion_personnel            # lance l'application (depuis ce dossier !)
make test                 # teste les modules SANS interface (résultats dans des fichiers .txt)
```

## 2. Comptes de test (fichier `users.txt`, format `identifiant;mot de passe;role;reference`)

| Identifiant | Mot de passe | Interface ouverte |
|---|---|---|
| `admin` | `admin123` | **Administrateur** |
| `PN001` | `pilote123` | Pilote |
| `PN002` | `copilote123` | Copilote |
| `PN003` | `hotesse123` | Hôtesse |
| `PN004` | `steward123` | Steward |

Les comptes de la 2e partie du tableau ouvrent l'**espace personnel navigant**. Le programme choisit l'interface selon le rôle du compte.

## 3. Interface ADMINISTRATEUR (3 onglets)

| Onglet | Contenu |
|---|---|
| **Gestion** | fiche complète ; boutons *Ajouter / Modifier / Supprimer / Vider / Exporter (.csv)* ; recherche (tous les champs ou un champ) ; tri A→Z / Z→A sur *Nom* ; filtres *Fonction* et *Statut* ; case « personnel en congé uniquement » |
| **Statistiques** | cartes (total, total des heures de vol) et barres de progression par *statut* |
| **Historique** | journal automatique de toutes les actions (date, heure, utilisateur, référence, action) ; recherche, filtre par action, export, effacement |

## 4. Interface UTILISATEUR : Mon espace

*Consultez votre fiche professionnelle et mettez à jour vos coordonnées.*

- **Recherche** : zone de saisie + choix du critère + boutons *Rechercher* / *Réinitialiser*.
- **Liste** : tableau (GtkTreeView) des résultats.
- **Ma fiche** : tous les champs de l'élément sélectionné.
- L'utilisateur ne voit **que ses propres données** (champ `matricule` = référence du compte dans `users.txt`).
- Il peut modifier lui-même : **Téléphone**, **Email** (bouton *Enregistrer mes modifications*, l'action est écrite dans l'historique).
- Aucun accès aux ajouts / suppressions / statistiques / historique (réservés à l'administrateur).

## 5. Structure de données et module

Le module porte le nom de la structure : **`Personnel`** -> `personnel.h` / `personnel.c`. Fichier de données : `personnel.txt` (une ligne par enregistrement, champs séparés par `;`).

| Champ | Libellé | Type | Obligatoire |
|---|---|---|---|
| `matricule` | Matricule | texte | oui |
| `nom` | Nom | texte | oui |
| `prenom` | Prénom | texte | oui |
| `cin` | CIN (8 chiffres) | texte | oui |
| `fonction` | Fonction | texte | oui |
| `base` | Base d'attache | texte | oui |
| `embauche` | Date d'embauche (JJ/MM/AAAA) | texte | oui |
| `heures` | Heures de vol | nombre | oui |
| `licence` | Numéro de licence | texte | oui |
| `expiration` | Expiration licence (JJ/MM/AAAA) | texte | oui |
| `tel` | Téléphone (8 chiffres) | texte | oui |
| `email` | Email | texte | oui |
| `statut` | Statut | texte | oui |

### Fonctions du module (aucun `printf` / `scanf` : elles retournent des codes)

| Fonction | Rôle | Retour |
|---|---|---|
| `ajouter_personnel(fichier, x)` | ajoute à la fin du fichier | 1 ok / 0 identifiant déjà utilisé / -2 valeur unique déjà utilisée / -1 erreur fichier |
| `modifier_personnel(fichier, id, x)` | remplace la ligne d'identifiant `id` | 1 / 0 introuvable / -2 / -1 |
| `supprimer_personnel(fichier, id)` | supprime la ligne | 1 / 0 introuvable / -1 |
| `chercher_personnel(fichier, id)` | retourne la structure (champ clé vide si introuvable) | - |
| `charger_personnel` / `sauvegarder_personnel` | lit / écrit tout le fichier | nombre lu / 1 |
| `filtrer_personnel(tab, n, res, texte, critere, val1, val2)` | recherche + 2 filtres exacts | nombre trouvé |
| `trier_personnel(tab, n, croissant)` | tri A→Z ou Z→A sur *Nom* | - |
| `compter_personnel(tab, n, valeur)` | nombre d'éléments dont *statut* = valeur | nombre |
| `total_personnel(tab, n)` | somme du champ *heures* | nombre |
| `exporter_personnel(fichier_csv, tab, n)` | fichier .csv ouvrable dans Excel | 1 / -1 |
| `valider_personnel(x)` + `message_erreur_personnel(code)` | contrôle de saisie | 0 valide, sinon n° du champ |

Module **`historique`** (`historique.h/.c`) : `ajouter_historique`, `charger_historique`, `filtrer_historique`, `compter_action`, `exporter_historique`, `vider_historique`.

## 6. Respect des consignes du devoir

| Consigne | Réponse |
|---|---|
| Implémenter les fonctions demandées | module `personnel.c` (ajouter, modifier, supprimer, chercher, filtrer, trier, compter, exporter, valider) |
| Travailler avec des fichiers texte | `personnel.txt`, `historique.txt`, `users.txt` |
| Nommer le module d'après la structure | `Personnel` -> `personnel.h` / `personnel.c` |
| Pas de `printf` / `scanf` dans les fonctions | aucun : codes de retour, l'affichage est fait par l'interface Glade (GtkTreeView) |
| Vérifier directement dans le fichier texte | `make test` : `test_personnel.txt`, `tri_personnel.txt`, `export_personnel.csv`, `rapport_personnel.txt` |

## 7. Fichiers du dossier

| Fichier | Rôle |
|---|---|
| `personnel.h`, `personnel.c` | module de la structure **Personnel** (logique + fichier texte) |
| `historique.h`, `historique.c` | module du journal |
| `test_personnel.c`, `test_historique.c` | tests sans interface |
| `gestion_personnel.glade` | interfaces Glade 2 : connexion, administrateur, utilisateur |
| `main.c`, `login.c` | démarrage et connexion (choix de l'interface selon le rôle) |
| `interface_admin.c` | espace administrateur |
| `interface_utilisateur.c` | espace utilisateur |
| `outils.c`, `app.h` | petites fonctions d'aide pour GTK |
| `personnel.txt`, `historique.txt`, `users.txt` | données de démonstration |
| `Makefile` | `make`, `make test`, `make clean` |
