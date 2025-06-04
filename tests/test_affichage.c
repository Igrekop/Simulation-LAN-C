#include <stdio.h>
#include "../include/reseau.h"
#include "../include/equipement.h"

int main() {
    printf("[TEST] Affichage d’une station\n");

    Station s = {
        .mac = 0x54d6a682c523,
        .ip = (130 << 24) | (79 << 16) | (80 << 8) | 21
    };
    afficher_station(s);

    printf("\n[TEST] Affichage d’un switch avec table\n");

    Switch sw = {
        .mac = 0x014523a6f7ab,
        .nb_ports = 4,
        .priorite = 1024
    };
    sw.table_commutation[0] = 0x001122334455;
    sw.table_commutation[2] = 0xaabbccddeeff;

    afficher_switch(sw);

    return 0;
}
