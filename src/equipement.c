#include <stdio.h>
#include <string.h>
#include "equipement.h"

int mac_vide(mac_addr_t mac) {
    for (int i = 0; i < MAC_ADDR_LEN; i++) {
        if (mac.addr[i] != 0) return 0;
    }
    return 1;
}

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

    // Affichage de la table de commutation
    int table_vide = 1;
    for (int i = 0; i < sw.nb_ports; i++) {
        if (!mac_vide(sw.mac_table[i])) {
            table_vide = 0;
            break;
        }
    }
    printf("  Table de commutation :\n");
    if (table_vide) {
        printf("    Table vide (aucune entrée apprise)\n");
    } else {
        for (int i = 0; i < sw.nb_ports; i++) {
            if (!mac_vide(sw.mac_table[i])) {
                printf("    Port %d → ", i);
                afficher_mac(sw.mac_table[i]);
                printf("\n");
            }
        }
    }
    // Affichage des connexions physiques
    printf("  Connexions physiques :\n");
    for (int p = 0; p < sw.nb_ports; p++) {
        if (sw.port_table[p] != -1) {
            printf("    Port %d → Equipement #%d\n", p, sw.port_table[p]);
        }
    }
}

void afficher_equipement(Equipement e) {
    if (e.type == STATION) {
        afficher_station(e.data.station);
    } else if (e.type == SWITCH) {
        afficher_switch(e.data.sw);
    }
}

void afficher_matrice_adjacence(const Reseau* reseau) {
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
            int poids = reseau->liens[i][j];
            if (poids == -1)
                printf("   .");
            else
                printf("%4d", poids);
        }
        printf("\n");
    }
    printf("\nLégende : . = pas de lien\n");
}

void afficher_reseau(const Reseau* reseau) {
    printf("    Réseau local :\n");
    printf("Nombre d'équipements : %d\n", reseau->nb_equipements);
    printf("\n   Équipements    \n");

    for (int i = 0; i < reseau->nb_equipements; i++) {
        printf("[Équipement #%d]\n", i);
        afficher_equipement(reseau->equipements[i]);
        printf("\n");
    }
}

// Compare deux MAC, retourne 1 si égales, 0 sinon
int mac_egal(mac_addr_t m1, mac_addr_t m2) {
    int i;
    for (i = 0; i < MAC_ADDR_LEN; i++) {
        if (m1.addr[i] != m2.addr[i]) return 0;
    }
    return 1;
}

// Compare deux IP, retourne 1 si égales, 0 sinon
int ip_egal(ip_addr_t ip1, ip_addr_t ip2) {
    int i;
    for (i = 0; i < IP_ADDR_LEN; i++) {
        if (ip1.addr[i] != ip2.addr[i]) return 0;
    }
    return 1;
}