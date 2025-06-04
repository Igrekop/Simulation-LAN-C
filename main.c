#include <stdio.h>
#include "equipement.h"
#include "reseau.h"

int main() {
    ReseauLocal r = {0};
    r.nb_equipements = 2;

    // Ajout d’un switch
    r.equipements[0].type = SWITCH;
    r.equipements[0].u.sw.mac = 0x014523a6f7ab;
    r.equipements[0].u.sw.nb_ports = 2;
    r.equipements[0].u.sw.priorite = 1024;
    r.equipements[0].u.sw.table_commutation[0] = 0x001122334455;

    // Ajout d’une station
    r.equipements[1].type = STATION;
    r.equipements[1].u.station.mac = 0x54d6a682c523;
    r.equipements[1].u.station.ip = (130 << 24) | (79 << 16) | (80 << 8) | 21;

    // Lien entre eux
    r.matrice_adjacence[0][1] = 4;
    r.matrice_adjacence[1][0] = 4;

    afficher_reseau(&r);
    return 0;
}
