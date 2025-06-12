#ifndef TRAME_H
#define TRAME_H

#include <stdint.h>
#include <stdio.h>
#include "equipement.h"

// Trame Ethernet simplifiée
typedef struct {
    uint8_t preambule[7];
    uint8_t SFD;
    mac_addr_t src_mac;
    mac_addr_t dest_mac;

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
mac_addr_t recevoir(const trame *t, equipement_t *e, int port_entree, reseau_t* reseau);

// Nouvelle fonction pour l'envoi de trame
void envoyer_trame(const trame *t, equipement_t *emetteur, mac_addr_t dest_mac, reseau_t* reseau);

#endif