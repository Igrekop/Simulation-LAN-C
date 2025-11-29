# Simulateur de Réseau

<h3 align="center">Simulateur de Réseau avec STP</h3>

<p align="center">
  Un simulateur de réseau en C permettant de modéliser des réseaux locaux avec stations et commutateurs, implémentant le protocole STP (Spanning Tree Protocol) et la commutation Ethernet.
  <br />
</p>

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Language: C](https://img.shields.io/badge/Language-C-blue.svg)](https://www.cprogramming.com/)

<img width="398" height="867" alt="image" src="https://github.com/user-attachments/assets/a6cac1c6-5e99-4116-9a43-8fb6dbfd5b04" />
<img width="748" height="634" alt="image" src="https://github.com/user-attachments/assets/73b21d56-eb05-4b7b-8b1e-6075e1bae497" />
<img width="842" height="485" alt="image" src="https://github.com/user-attachments/assets/ab9ec835-0bc6-40e1-8a43-243fc9540e00" />


<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table des matières</summary>
  <ol>
    <li><a href="#à-propos-du-projet">À propos du projet</a></li>
    <li>
      <a href="#pour-commencer">Pour commencer</a>
      <ul>
        <li><a href="#prérequis">Prérequis</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#utilisation">Utilisation</a></li>
    <li><a href="#fonctionnalités">Fonctionnalités</a></li>
    <li><a href="#structure-du-projet">Structure du projet</a></li>
    <li><a href="#format-de-configuration">Format de configuration</a></li>
    <li><a href="#licence">Licence</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## À propos du projet

Ce simulateur de réseau permet de modéliser et de simuler le comportement d'un réseau local (LAN) composé de :
- **Stations** : équipements terminaux avec adresses MAC et IP
- **Commutateurs (Switches)** : équipements de commutation avec tables MAC et gestion des ports

Le simulateur implémente :
- Le protocole **STP (Spanning Tree Protocol)** pour éviter les boucles dans le réseau
- La **commutation Ethernet** avec apprentissage des adresses MAC
- La **propagation de trames** entre stations via les commutateurs
- L'affichage des **tables MAC** et de l'**état des ports STP**

### Construit avec

* [![C](https://img.shields.io/badge/Language-C-blue.svg)](https://www.cprogramming.com/)
* GCC avec flags de compilation stricts (`-Wall -Wextra -Werror`)

<!-- GETTING STARTED -->
## Pour commencer

### Prérequis

* **GCC** (GNU Compiler Collection) - version récente recommandée
* **Make** pour la compilation
* Un terminal compatible avec les codes ANSI (pour l'affichage coloré)

### Installation

1. Clonez le dépôt
   ```sh
   git clone <url-du-depot>
   cd sae23_s21_dublanc-soubigou_petit_salmane
   ```

2. Compilez le projet
   ```sh
   make
   ```

3. Lancez le simulateur
   ```sh
   make run
   ```
   ou directement :
   ```sh
   ./bin/simulateur_reseau reseau_config.txt
   ```

4. Nettoyage des fichiers compilés
   ```sh
   make clean
   ```

<!-- USAGE -->
## Utilisation

Le simulateur propose un menu interactif avec les options suivantes :

1. **Lancer une simulation de trame**
   - Sélectionnez une station source et une station destination
   - Entrez les données à envoyer
   - Le simulateur affiche le chemin de la trame et l'apprentissage des adresses MAC

2. **Afficher l'état des ports STP**
   - Affiche l'état de chaque port (actif/bloqué) après calcul du spanning tree

3. **Afficher les tables MAC**
   - Affiche les tables MAC de tous les commutateurs avec les adresses apprises

4. **Afficher la matrice d'adjacence**
   - Affiche la topologie du réseau sous forme de matrice

<!-- FEATURES -->
## Fonctionnalités

### Protocole STP (Spanning Tree Protocol)
- Calcul automatique de l'arbre couvrant pour éviter les boucles
- Gestion des priorités des commutateurs
- Blocage/déblocage des ports selon l'arbre couvrant

### Commutation Ethernet
- Apprentissage automatique des adresses MAC
- Tables MAC par commutateur
- Inondation (flooding) lorsque la destination est inconnue
- Commutation directe lorsque la destination est connue

### Simulation de trames
- Création de trames Ethernet complètes
- Propagation dans le réseau selon STP
- Affichage détaillé du chemin de la trame
- Décodage des données (hexadécimal, ASCII, décimal)

### Affichage
- Interface en couleur avec codes ANSI
- Affichage formaté des tables MAC
- Visualisation de la matrice d'adjacence
- Détails des trames Ethernet

<!-- PROJECT STRUCTURE -->
## Structure du projet

```
.
├── README.md
├── Makefile
├── reseau_config.txt          # Fichier de configuration du réseau
├── include/                   # Fichiers d'en-tête
│   ├── equipement.h           # Structures pour équipements (stations, switches)
│   ├── reseau.h               # Structure du réseau et chargement
│   ├── trame.h                # Structures et fonctions pour trames Ethernet
│   ├── commutation.h          # Fonctions de commutation
│   ├── stp.h                  # Protocole Spanning Tree
│   └── affichage.h            # Fonctions d'affichage
├── src/                       # Fichiers source
│   ├── main.c                 # Point d'entrée et menu principal
│   ├── equipement.c           # Fonctions de base pour équipements
│   ├── reseau.c               # Chargement et gestion du réseau
│   ├── trame.c                # Création et affichage de trames
│   ├── commutation.c          # Logique de commutation
│   ├── stp.c                  # Implémentation STP
│   └── affichage.c            # Fonctions d'affichage
└── bin/                       # Fichiers compilés (générés)
    └── simulateur_reseau      # Exécutable
```

<!-- CONFIGURATION FORMAT -->
## Format de configuration

Le fichier de configuration (`reseau_config.txt`) suit le format suivant :

```
<nb_switches> <nb_stations>
<lignes_switches>
<lignes_stations>
<lignes_liens>
```

### Format des lignes

**Switches** (type 2) :
```
2;<MAC>;<nb_ports>;<priorité>
```
Exemple : `2;01:45:23:a6:f7:01;8;1024`

**Stations** (type 1) :
```
1;<MAC>;<IP>
```
Exemple : `1;54:d6:a6:82:c5:01;130.79.80.1`

**Liens** (type 0) :
```
0;<equip1>;<equip2>;<poids>
```
Exemple : `0;1;4;4` (liaison entre équipement 1 et 4 avec poids 4)

### Exemple de fichier

```
15 11
2;01:45:23:a6:f7:01;8;1024
2;01:45:23:a6:f7:02;8;1024
...
1;54:d6:a6:82:c5:01;130.79.80.1
1;54:d6:a6:82:c5:02;130.79.80.2
...
0;1;4;4
0;2;4;4
...
```

<!-- LICENSE -->
## Licence

Distribué sous licence MIT. Voir `LICENSE.txt` pour plus de détails.

<!-- CONTACT -->
## Contact

yvann.du.soub@gmail.com

---

**Note** : Ce projet est un simulateur éducatif et ne prétend pas être une implémentation complète des protocoles réseau réels.
