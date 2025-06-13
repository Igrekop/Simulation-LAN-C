#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "reseau.h"
#include "equipement.h"

void afficher_mac(mac_addr_t mac) {
    for (int i = 0; i < MAC_ADDR_LEN; i++) {
        printf("%02x", mac.addr[i]);
        if (i < MAC_ADDR_LEN-1) printf(":");
    }
}

void afficher_ip(ip_addr_t ip) {
    for (int i = 0; i < IP_ADDR_LEN; i++) {
        printf("%d", ip.addr[i]);
        if (i < IP_ADDR_LEN-1) printf(".");
    }
}

// convertit le string en mac_addr_t
mac_addr_t convertir_en_mac(const char* mac_str) {
    mac_addr_t mac;
    unsigned int bytes[MAC_ADDR_LEN];
    sscanf(mac_str, "%x:%x:%x:%x:%x:%x",
           &bytes[0], &bytes[1], &bytes[2], &bytes[3], &bytes[4], &bytes[5]);
    for (int i = 0; i < MAC_ADDR_LEN; i++) mac.addr[i] = (uint8_t)bytes[i];
    return mac;
}

// convertit le string en ip_addr_t
ip_addr_t convertir_en_ip(const char* ip_str) {
    ip_addr_t ip;
    unsigned int bytes[IP_ADDR_LEN];
    sscanf(ip_str, "%u.%u.%u.%u", &bytes[0], &bytes[1], &bytes[2], &bytes[3]);
    for (int i = 0; i < IP_ADDR_LEN; i++) ip.addr[i] = (uint8_t)bytes[i];
    return ip;
}

// Convertit une chaîne xx:yy:zz:... en mac_addr_t
mac_addr_t parse_mac(const char *str) {
    mac_addr_t mac;
    unsigned int bytes[MAC_ADDR_LEN];
    sscanf(str, "%x:%x:%x:%x:%x:%x",
           &bytes[0], &bytes[1], &bytes[2], &bytes[3], &bytes[4], &bytes[5]);
    for (int i = 0; i < MAC_ADDR_LEN; i++) mac.addr[i] = (uint8_t)bytes[i];
    return mac;
}

// Convertit une chaîne a.b.c.d en ip_addr_t
ip_addr_t parse_ip(const char *str) {
    ip_addr_t ip;
    unsigned int bytes[IP_ADDR_LEN];
    sscanf(str, "%u.%u.%u.%u", &bytes[0], &bytes[1], &bytes[2], &bytes[3]);
    for (int i = 0; i < IP_ADDR_LEN; i++) ip.addr[i] = (uint8_t)bytes[i];
    return ip;
}

// Fonction qui récupère le fichier appelé (réseau_config) et le lit
// Faire les erreurs
int charger_reseau_fichier(const char* nom_fichier, reseau_t* reseau) {
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
        reseau->equipements[i].type = (equip_type_t)type_equipement_lue; // associe la valeure au nom de l'equip

        // lit en fonction de l'equipement
        if (reseau->equipements[i].type == SWITCH) {
            sscanf(line_buffer + 2, "%17[^;];%d;%d", mac_str, &nb_ports_lue, &priorite_lue);
            reseau->equipements[i].data.sw.mac = convertir_en_mac(mac_str);
            reseau->equipements[i].data.sw.nb_ports = nb_ports_lue;
            reseau->equipements[i].data.sw.priority = priorite_lue;
            for(int j=0; j<MAX_PORTS; j++) {
                reseau->equipements[i].data.sw.mac_table[j] = (mac_addr_t){0};
                // table de commutation pas encore faites
            }
            

        } else if (reseau->equipements[i].type == STATION) {
            sscanf(line_buffer + 2, "%17[^;];%15s", mac_str, ip_str);
            reseau->equipements[i].data.station.mac = convertir_en_mac(mac_str);
            reseau->equipements[i].data.station.ip = convertir_en_ip(ip_str);
        }
    }
    for (int i = 0; i < reseau->nb_equipements; i++) {
        for (int j = 0; j < reseau->nb_equipements; j++) {
            reseau->liens[i].equip1 = -1;
            reseau->liens[i].equip2 = -1;
            reseau->liens[i].poids = -1;
        }
    }
    for (int k = 0; k < nb_liens_lus; k++) {
        int eq1, eq2, poids;
        fscanf(fichier, "%d;%d;%d\n", &eq1, &eq2, &poids);
        reseau->liens[k].equip1 = eq1;
        reseau->liens[k].equip2 = eq2;
        reseau->liens[k].poids = poids;
        // idem
    }

    // Parcours des liens et mise à jour des ports physiques
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type != SWITCH) continue;
        switch_t* sw = &reseau->equipements[i].data.sw;

        int port = 0;
        for (int j = 0; j < reseau->nb_equipements; j++) {
            if (reseau->liens[i].equip2 >= 0) {
                if (port < sw->nb_ports) {
                    sw->port_table[port++] = j; // j = index équipement connecté
                }
            }
        }

        // Initialise les ports restants
        for (; port < MAX_PORTS; port++) {
            sw->port_table[port] = -1;
        }
    }

    fclose(fichier);
    return 0;
}

void afficher_table_commutation(switch_t* sw) {
    printf("Table de commutation pour le switch (MAC → Port):\n");
    int table_vide = 1;
    for (int i = 0; i < sw->nb_ports; i++) {
        if (!mac_egal(sw->mac_table[i], (mac_addr_t){{0}})) {
            table_vide = 0;
            break;
        }
    }
    if (table_vide) {
        printf("  Table vide (aucune entrée apprise)\n");
    } else {
        for (int i = 0; i < sw->nb_ports; i++) {
            if (!mac_egal(sw->mac_table[i], (mac_addr_t){{0}})) {
                printf("  Port %d → MAC: ", i);
                afficher_mac(sw->mac_table[i]);
                printf("\n");
            }
        }
    }
}

// Initialise un réseau vide
void init_reseau(reseau_t* reseau) {
    reseau->nb_equipements = 0;
    reseau->nb_liens = 0;
}

// Ajoute une station au réseau
int ajouter_station(reseau_t* reseau, mac_addr_t mac, ip_addr_t ip) {
    if (reseau->nb_equipements >= MAX_STATIONS + MAX_SWITCHES) {
        printf("Erreur : réseau plein\n");
        return 0;
    }

    equipement_t e;
    e.type = STATION;
    e.data.station.mac = mac;
    e.data.station.ip = ip;
    reseau->equipements[reseau->nb_equipements++] = e;
    return 1;
}

// Ajoute un switch au réseau
int ajouter_switch(reseau_t* reseau, mac_addr_t mac, int nb_ports, int priority) {
    if (reseau->nb_equipements >= MAX_STATIONS + MAX_SWITCHES) {
        printf("Erreur : réseau plein\n");
        return 0;
    }

    equipement_t e;
    e.type = SWITCH;
    e.data.sw.mac = mac;
    e.data.sw.nb_ports = nb_ports;
    e.data.sw.priority = priority;
    reseau->equipements[reseau->nb_equipements++] = e;
    return 1;
}

// Ajoute un lien entre deux équipements
int ajouter_lien(reseau_t* reseau, int equip1, int equip2, int poids) {
    if (reseau->nb_liens >= 128) {
        printf("Erreur : nombre maximum de liens atteint\n");
        return 0;
    }

    if (equip1 < 0 || equip1 >= reseau->nb_equipements ||
        equip2 < 0 || equip2 >= reseau->nb_equipements) {
        printf("Erreur : équipement invalide\n");
        return 0;
    }

    reseau->liens[reseau->nb_liens].equip1 = equip1;
    reseau->liens[reseau->nb_liens].equip2 = equip2;
    reseau->liens[reseau->nb_liens].poids = poids;
    reseau->nb_liens++;
    return 1;
}

// Charge le réseau depuis un fichier
int charger_reseau(const char *filename, reseau_t *reseau) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Erreur ouverture fichier");
        return -1;
    }

    int nb_equipements, nb_liens;
    if (fscanf(file, "%d %d\n", &nb_equipements, &nb_liens) != 2) {
        fclose(file);
        return -2;
    }
    reseau->nb_equipements = nb_equipements;
    reseau->nb_liens = nb_liens;

    // Lecture des équipements
    char line[256];
    for (int i = 0; i < nb_equipements; i++) {
        if (!fgets(line, sizeof(line), file)) {
            fclose(file);
            return -3;
        }
        int type = -1;
        sscanf(line, "%d;", &type);
        char *ptr = strchr(line, ';');
        if (!ptr) continue;
        ptr++; // après le premier ';'

        if (type == 1) { // station
            char mac_str[32], ip_str[32];
            sscanf(ptr, "%31[^;];%31[^\n]", mac_str, ip_str);
            reseau->equipements[i].type = STATION;
            reseau->equipements[i].data.station.mac = parse_mac(mac_str);
            reseau->equipements[i].data.station.ip = parse_ip(ip_str);
        } else if (type == 2) { // switch
            char mac_str[32];
            int nb_ports, priority;
            sscanf(ptr, "%31[^;];%d;%d", mac_str, &nb_ports, &priority);
            reseau->equipements[i].type = SWITCH;
            reseau->equipements[i].data.sw.mac = parse_mac(mac_str);
            reseau->equipements[i].data.sw.nb_ports = nb_ports;
            reseau->equipements[i].data.sw.priority = priority;
        } else {
            fclose(file);
            return -4;
        }
    }

    // Lecture des liens
    for (int i = 0; i < nb_liens; i++) {
        if (!fgets(line, sizeof(line), file)) {
            fclose(file);
            return -5;
        }
        int e1, e2, poids;
        if (sscanf(line, "%d;%d;%d", &e1, &e2, &poids) != 3) {
            fclose(file);
            return -6;
        }
        reseau->liens[i].equip1 = e1;
        reseau->liens[i].equip2 = e2;
        reseau->liens[i].poids = poids;
    }
    // Initialisation des tables de voisins pour chaque switch
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == SWITCH) {
            reseau->equipements[i].data.sw.nb_ports = 0;
        }
    }
    for (int i = 0; i < reseau->nb_liens; i++) {
        int e1 = reseau->liens[i].equip1;
        int e2 = reseau->liens[i].equip2;

        // Pour e1
        if (reseau->equipements[e1].type == SWITCH) {
            switch_t *sw1 = &reseau->equipements[e1].data.sw;
            sw1->port_table[sw1->nb_ports] = e2;
            if (reseau->equipements[e2].type == STATION)
                sw1->port_etat[sw1->nb_ports] = 1; // Port vers station toujours actif
            else
                sw1->port_etat[sw1->nb_ports] = 0; // Port vers switch, déterminé par STP
            sw1->nb_ports++;
        }
        // Pour e2
        if (reseau->equipements[e2].type == SWITCH) {
            switch_t *sw2 = &reseau->equipements[e2].data.sw;
            sw2->port_table[sw2->nb_ports] = e1;
            if (reseau->equipements[e1].type == STATION)
                sw2->port_etat[sw2->nb_ports] = 1; // Port vers station toujours actif
            else
                sw2->port_etat[sw2->nb_ports] = 0; // Port vers switch, déterminé par STP
            sw2->nb_ports++;
        }
    }
    fclose(file);
    return 0;
}