#include <stdio.h>
#include "equipement.h"

void afficher_station(Station s) {
    printf("Station\n");
    printf("  MAC : "); afficher_mac(s.mac); printf("\n");
    printf("  IP  : "); afficher_ip(s.ip); printf("\n");
}

void afficher_switch(Switch sw) {
    printf("Switch\n");
    printf("  MAC      : "); afficher_mac(sw.mac); printf("\n");
    printf("  Ports    : %d\n", sw.nb_ports);
    printf("  Priorité : %d\n", sw.priorite);
    printf("  Table de commutation :\n");
    for (int i = 0; i < sw.nb_ports; i++) {
        if (sw.table_commutation[i] != 0) { // 0 = port vide
            printf("    Port %d → ", i);
            afficher_mac(sw.table_commutation[i]);
            printf("\n");
        }
        // faire affichage table
    }
}

void afficher_equipement(Equipement e) {
    if (e.type == STATION) {
        afficher_station(e.typequipement.station);
    } else if (e.type == SWITCH) {
        afficher_switch(e.typequipement.sw);
    }
}

void afficher_reseau(const ReseauLocal* reseau) {
    printf("=== Réseau local ===\n");
    printf("Nombre d'équipements : %d\n", reseau->nb_equipements);
    printf("\n-- Équipements --\n");

    for (int i = 0; i < reseau->nb_equipements; i++) {
        printf("[Équipement #%d]\n", i);
        afficher_equipement(reseau->equipements[i]);
        printf("\n");
    }

    // Faire affichage table de commutations sous la forme
    // Entrée 1 --> Sortie 0
    // Entrée 0 --> Sortie 2
}