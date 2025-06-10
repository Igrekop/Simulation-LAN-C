#ifndef TRAME_H
#define TRAME_H

#include <stdint.h>
#include <stdio.h>

// Trame Ethernet simplifiée
typedef struct {
    uint8_t preambule[7];
    uint8_t SFD;
    uint8_t dest_mac[6];
    uint8_t src_mac[6];
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

#endif
