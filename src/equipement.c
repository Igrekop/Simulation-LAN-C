#include "affichage.h"
#include "equipement.h"
#include <stdio.h>
#include <string.h>

int mac_vide(mac_addr_t mac) {
    for (int i = 0; i < MAC_ADDR_LEN; i++) {
        if (mac.addr[i] != 0) return 0;
    }
    return 1;
}

void afficher_station(station_t s) {
    printf("MAC: ");
    afficher_mac(s.mac);
    printf(" | IP: ");
    afficher_ip(s.ip);
    printf("\n");
}

void afficher_switch(switch_t sw) {
    printf("MAC: ");
    afficher_mac(sw.mac);
    printf(" | Ports: %d | Priorité: %d\n", sw.nb_ports, sw.priority);
}

void afficher_equipement(equipement_t e) {
    if (e.type == STATION) {
        printf("Station - ");
        afficher_station(e.data.station);
    } else {
        printf("Switch - ");
        afficher_switch(e.data.sw);
    }
}

void afficher_matrice_adjacence(const reseau_t* reseau) {
    printf("\nMatrice d'adjacence :\n");
    printf("    ");
    for (int i = 0; i < reseau->nb_equipements; i++) {
        printf("%2d ", i);
    }
    printf("\n");

    for (int i = 0; i < reseau->nb_equipements; i++) {
        printf("%2d  ", i);
        for (int j = 0; j < reseau->nb_equipements; j++) {
            int poids = -1;
            for (int k = 0; k < reseau->nb_liens; k++) {
                if ((reseau->liens[k].equip1 == i && reseau->liens[k].equip2 == j) ||
                    (reseau->liens[k].equip1 == j && reseau->liens[k].equip2 == i)) {
                    poids = reseau->liens[k].poids;
                    break;
                }
            }
            printf("%2d ", poids);
        }
        printf("\n");
    }
}

void afficher_reseau(const reseau_t* reseau) {
    printf("\n=== Réseau ===\n");
    printf("Nombre d'équipements : %d\n", reseau->nb_equipements);
    printf("Nombre de liens : %d\n", reseau->nb_liens);
    printf("\nÉquipements :\n");
    for (int i = 0; i < reseau->nb_equipements; i++) {
        printf("Équipement %d : ", i);
        afficher_equipement(reseau->equipements[i]);
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