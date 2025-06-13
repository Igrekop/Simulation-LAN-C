#include "affichage.h"
#include "equipement.h"
#include <stdio.h>

// Affiche une adresse MAC en notation hexadécimale
void afficher_mac(mac_addr_t mac) {
    for (int i = 0; i < MAC_ADDR_LEN; i++) {
        printf("%02x", mac.addr[i]);
        if (i < MAC_ADDR_LEN-1) printf(":");
    }
}

// Affiche une adresse IP en notation décimale pointée
void afficher_ip(ip_addr_t ip) {
    for (int i = 0; i < IP_ADDR_LEN; i++) {
        printf("%d", ip.addr[i]);
        if (i < IP_ADDR_LEN-1) printf(".");
    }
}

// Affichage d'une station
void afficher_station(station_t s) {
    printf("MAC: ");
    afficher_mac(s.mac);
    printf(" | IP: ");
    afficher_ip(s.ip);
    printf("\n");
}

// Affichage d'un switch
void afficher_switch(switch_t sw) {
    printf("MAC: ");
    afficher_mac(sw.mac);
    printf(" | Ports: %d | Priorité: %d\n", sw.nb_ports, sw.priority);
}

// Affichage générique d'un équipement
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