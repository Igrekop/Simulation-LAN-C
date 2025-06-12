#ifndef TRAME_H
#define TRAME_H

#include <stdint.h>
#include <stdio.h>
#include "reseau.h"

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

// Fonctions d'affichage
void afficher_trame(const trame *t);
void afficher_trame_complete(const trame *t);
AdresseMAC recevoir(const trame *t, Equipement *e, int port_entree, ReseauLocal* reseau);

// Nouvelle fonction pour l'envoi de trame
void envoyer_trame(const trame *t, Equipement *emetteur, AdresseMAC dest_mac, ReseauLocal* reseau);

#endif