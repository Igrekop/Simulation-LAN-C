#ifndef EQUIPEMENT_H
#define EQUIPEMENT_H

#include <stdio.h>
#include <stdint.h>
#include "reseau.h"

#define MAX_PORTS 32
#define MAX_EQUIPES 100

// Station
typedef struct {
    AdresseMAC mac;
    AdresseIP ip;
} Station;

// Switch
typedef struct {
    AdresseMAC mac;
    int nb_ports;
    int priorite;
    AdresseMAC table_commutation[MAX_PORTS];
} Switch;

// Type d'équipement
typedef enum {
    STATION = 1, 
    SWITCH = 2
} TypeEquipement;

typedef struct {
    TypeEquipement type;
    union {
        Station station;
        Switch sw;
    } typequipement;
} Equipement;

typedef struct ReseauLocal {
    Equipement equipements[MAX_EQUIPES];
    int nb_equipements;
    int matrice_adjacence[MAX_EQUIPES][MAX_EQUIPES]; // Pour faire la table de commutations
} ReseauLocal;

// Fonctions d'affichage
void afficher_station(Station s);
void afficher_switch(Switch sw);
void afficher_equipement(Equipement e);
void afficher_reseau(const ReseauLocal* reseau);

#endif
