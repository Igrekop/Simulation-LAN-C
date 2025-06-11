#ifndef TRAME_H
#define TRAME_H

#include <stdint.h>
#include <stdio.h>
#include "reseau.h"
#include "port.h"

// Trame Ethernet simplifiée
typedef struct {
    uint8_t preambule[7];
    uint8_t SFD;
    AdresseMAC src_mac;
    AdresseMAC dest_mac;

    uint16_t ethertype;
    union {
        uint8_t raw[1500];
        struct {
            uint8_t data[46];
            uint8_t padding[1454];
        } contenu;
    } DATA;
    uint32_t FCS;
} trame;

// Constantes pour les adresses MAC spéciales
extern const AdresseMAC ADRESSE_BROADCAST;

// Fonctions d'affichage
void afficher_trame(const trame *t);
void afficher_trame_complete(const trame *t);
AdresseMAC recevoir(const trame *t, Equipement *e);

// Fonctions de traitement des trames
void initialiser_trame(trame *t, const AdresseMAC *src, const AdresseMAC *dest);
int est_broadcast(const AdresseMAC *mac);
void traiter_trame_station(const trame *t, Equipement *station, PortManager *pm);
void traiter_trame_switch(const trame *t, Equipement *sw, PortManager *pm, int port_reception);
void envoyer_trame(const trame *t, Equipement *src, PortManager *pm, ReseauLocal *reseau);

#endif