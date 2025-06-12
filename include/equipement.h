#ifndef EQUIPEMENT_H
#define EQUIPEMENT_H

#include <stdio.h>
#include <stdint.h>
#include "reseau.h"

#define MAC_ADDR_LEN 6
#define IP_ADDR_LEN 4
#define MAX_PORTS 64
#define MAX_STATIONS 32
#define MAX_SWITCHES 16

// Types d'adresses
typedef struct {
    uint8_t addr[MAC_ADDR_LEN];
} AdresseMAC;

typedef struct {
    uint8_t addr[IP_ADDR_LEN];
} AdresseIP;

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
    AdresseMAC mac_table[MAX_PORTS];
    int port_table[MAX_PORTS]; // index du voisin
    int port_etat[MAX_PORTS];  // 1 = actif (spanning tree), 0 = bloqué
    int mac_table_size;
} Switch;

// Type d'équipement
typedef enum { STATION, SWITCH } typequipement;

typedef struct {
    typequipement type;
    union {
        Station station;
        Switch sw;
    } data;
} Equipement;

// Lien entre équipements
typedef struct {
    int equip1;
    int equip2;
    int poids;
} Lien;

// Réseau
typedef struct {
    int nb_equipements;
    equipement_t equipements[MAX_SWITCHES + MAX_STATIONS];
    int nb_liens;
    lien_t liens[128];
} reseau_t;

// Fonctions simples de comparaison
int mac_egal(AdresseMAC m1, AdresseMAC m2);
int ip_egal(ip_addr_t ip1, ip_addr_t ip2);

// Fonctions d'affichage
void afficher_station(station_t s);
void afficher_switch(switch_t sw);
void afficher_equipement(equipement_t e);
void afficher_reseau(const reseau_t* reseau);
void afficher_matrice_adjacence(const reseau_t* reseau);

#endif
