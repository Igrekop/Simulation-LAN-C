#include <stdio.h>
#include "../include/equipement.h"

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
    printf("  Table de commutation : (encore à faire)\n");
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

void afficher_matrice_adjacence(const ReseauLocal* reseau) {
    printf("-- Matrice d'adjacence (coût des liens) --\n\n    ");
    for (int i = 0; i < reseau->nb_equipements; i++) {
        printf("%4d", i);
    }
    printf("\n   ");
    for (int i = 0; i < reseau->nb_equipements; i++) {
        printf("----");
    }
    printf("\n");

    for (int i = 0; i < reseau->nb_equipements; i++) {
        printf("%3d|", i);
        for (int j = 0; j < reseau->nb_equipements; j++) {
            int poids = reseau->matrice_adjacence[i][j];
            if (poids == -1)
                printf("   .");
            else
                printf("%4d", poids);
        }
        printf("\n");
    }
    printf("\nLégende : . = pas de lien\n");
}

void afficher_reseau(const ReseauLocal* reseau) {
    printf("    Réseau local :\n");
    printf("Nombre d'équipements : %d\n", reseau->nb_equipements);
    printf("\n   Équipements    \n");

    for (int i = 0; i < reseau->nb_equipements; i++) {
        printf("[Équipement #%d]\n", i);
        afficher_equipement(reseau->equipements[i]);
        printf("\n");
    }
}