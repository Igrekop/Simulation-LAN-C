#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "reseau.h"
#include "equipement.h"

void afficher_mac(AdresseMAC mac) {
    for (int i = 5; i >= 0; i--) {
        unsigned int octet = (mac >> (8 * i)) & 0xFF;
        printf("%02x", octet);
        if (i > 0) printf(":");
    }
}

void afficher_ip(AdresseIP ip) {
    for (int i = 3; i >= 0; i--) {
        printf("%d", (ip >> (8 * i)) & 0xFF);
        if (i > 0) printf(".");
    }
}

// convertit le string en uint64_t
AdresseMAC convertir_en_mac(const char* mac_str) {
    AdresseMAC mac = 0;
    unsigned int octets[6];
    sscanf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
            &octets[0], &octets[1], &octets[2],
            &octets[3], &octets[4], &octets[5]);

    for (int i = 0; i < 6; i++) {
        mac = (mac << 8) | octets[i];
    }
    return mac;
}
// convertit le string en uint32_t
AdresseIP convertir_en_ip(const char* ip_str) {
    AdresseIP ip = 0;
    unsigned int octets[4];
    sscanf(ip_str, "%d.%d.%d.%d",
            &octets[0], &octets[1], &octets[2], &octets[3]);

    ip = (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | octets[3];
    return ip;
}


// Fonction qui récupère le fichier appelé (réseau_config) et le lit
// Faire les erreurs
int charger_reseau_fichier(const char* nom_fichier, ReseauLocal* reseau) {
    FILE* fichier = fopen(nom_fichier, "r"); // faire gestion d'erreur car sinn ca risque de planter

    int nb_equipements_lus; // dans le ficher
    int nb_liens_lus; // idem
    fscanf(fichier, "%d %d\n", &nb_equipements_lus, &nb_liens_lus); // lecture du haut du fichier (15 11)

    reseau->nb_equipements = nb_equipements_lus; // met dans le réseau le nombre actuel

    char line_buffer[256]; // tableau de char qui lit une ligne entière du fichier

    for (int i = 0; i < reseau->nb_equipements; i++) {
        fgets(line_buffer, sizeof(line_buffer), fichier);
        line_buffer[strcspn(line_buffer, "\n")] = '\0';

        int type_equipement_lue;
        char mac_str[20];
        char ip_str[20];
        int nb_ports_lue, priorite_lue;

        sscanf(line_buffer, "%d;", &type_equipement_lue); // lit le premier nombre jusqu a : dans "XX:XX"
        reseau->equipements[i].type = (TypeEquipement)type_equipement_lue; // associe la valeure au nom de l'equip

        // lit en fonction de l'equipement
        if (reseau->equipements[i].type == SWITCH) {
            sscanf(line_buffer + 2, "%17[^;];%d;%d", mac_str, &nb_ports_lue, &priorite_lue);
            reseau->equipements[i].typequipement.sw.mac = convertir_en_mac(mac_str);
            reseau->equipements[i].typequipement.sw.nb_ports = nb_ports_lue;
            reseau->equipements[i].typequipement.sw.priorite = priorite_lue;
            for(int j=0; j<MAX_PORTS; j++) {
                reseau->equipements[i].typequipement.sw.table_commutation[j] = 0;
                // table de commutation pas encore faites
            }

        } else if (reseau->equipements[i].type == STATION) {
            sscanf(line_buffer + 2, "%17[^;];%15s", mac_str, ip_str);
            reseau->equipements[i].typequipement.station.mac = convertir_en_mac(mac_str);
            reseau->equipements[i].typequipement.station.ip = convertir_en_ip(ip_str);
        }
    }
    for (int i = 0; i < reseau->nb_equipements; i++) {
        for (int j = 0; j < reseau->nb_equipements; j++) {
            reseau->matrice_adjacence[i][j] = -1;
        }
    }
    for (int k = 0; k < nb_liens_lus; k++) {
        int eq1, eq2, poids;
        fscanf(fichier, "%d;%d;%d\n", &eq1, &eq2, &poids);
        reseau->matrice_adjacence[eq1][eq2] = poids;
        reseau->matrice_adjacence[eq2][eq1] = poids;
        // idem
    }

    fclose(fichier);
    return 0;
}