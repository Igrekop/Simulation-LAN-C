#include "equipement.h"
#include "affichage.h"
#include "reseau.h"
#include "trame.h"
#include "commutation.h"
#include "stp.h"
#include <stdio.h>
#include <stdlib.h> 

#define NB_MAX_EQUIPEMENTS 64


// Recherche d'une station par IP
int trouver_station_par_ip(reseau_t *reseau, ip_addr_t ip) {
    int i;
    for (i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == STATION &&
            ip_egal(reseau->equipements[i].data.station.ip, ip)) {
            return i;
        }
    }
    return -1;
}

// Propagation de la trame avec anti-boucle simple
int propager_trame(
    reseau_t *reseau,
    int courant,
    int precedent,
    const ethernet_frame_t *trame,
    int dest_station,
    int *trouve,
    int profondeur,
    int *visited
) {
    if (profondeur > reseau->nb_equipements) return 0;
    if (*trouve) return 1;
    if (visited[courant]) return 0;
    visited[courant] = 1;

    if (courant == dest_station) {
        printf("La trame arrive à la station destination (%d) !\n", courant);
        *trouve = 1;
        return 1;
    }

    equipement_t *eq = &reseau->equipements[courant];
    if (eq->type == SWITCH) {
        switch_t *sw = &eq->data.sw;
        if (precedent != -1) {
            int port_enregistre = switch_rechercher_port(sw, trame->src);
            if (port_enregistre == -1) {
                printf("Switch %d apprend MAC source : ", courant);
                afficher_mac(trame->src);
                printf(" sur port (voisin) %d\n", precedent);
                switch_apprendre_mac(sw, trame->src, precedent);
            }
        }

        int port_equip = switch_rechercher_port(sw, trame->dest);
        if (port_equip != -1 && port_equip != precedent) {
            printf("Switch %d : MAC destination connue, envoie vers équipement %d\n", courant, port_equip);
            propager_trame(reseau, port_equip, courant, trame, dest_station, trouve, profondeur + 1, visited);
            return 1;
        } else {
            printf("Switch %d : MAC destination inconnue, inonde tous les ports\n", courant);
            int i;
            for (i = 0; i < reseau->nb_liens; i++) {
                int e1 = reseau->liens[i].equip1, e2 = reseau->liens[i].equip2;
                int voisin = (e1 == courant) ? e2 : (e2 == courant ? e1 : -1);
                if (voisin != -1 && voisin != precedent) {
                    propager_trame(reseau, voisin, courant, trame, dest_station, trouve, profondeur + 1, visited);
                    if (*trouve) return 1;
                }
            }
        }
    }
    return 0;
}

// Simulation d'une trame entre deux stations
void simuler_trame_station(reseau_t *reseau, int idx_src, int idx_dest) {
    printf("\n--- Simulation d'une trame de la station %d vers la station %d ---\n", idx_src, idx_dest);

    // Basic validation for station type
    if (reseau->equipements[idx_src].type != STATION || reseau->equipements[idx_dest].type != STATION) {
        printf("Erreur: L'indice source ou destination ne correspond pas à une station.\n");
        return;
    }

    station_t src = reseau->equipements[idx_src].data.station;
    station_t dest = reseau->equipements[idx_dest].data.station;
    uint8_t data[] = {0xde, 0xad, 0xbe, 0xef};
    ethernet_frame_t trame;
    creer_trame_ethernet(&trame, src.mac, dest.mac, 0x0800, data, sizeof(data));
    afficher_trame_utilisateur(&trame);

    int trouve = 0;
    int visited[NB_MAX_EQUIPEMENTS] = {0};
    int i;
    for (i = 0; i < reseau->nb_liens; i++) {
        int e1 = reseau->liens[i].equip1, e2 = reseau->liens[i].equip2;
        int voisin = (e1 == idx_src) ? e2 : (e2 == idx_src ? e1 : -1);
        if (voisin != -1) {
            propager_trame(reseau, voisin, idx_src, &trame, idx_dest, &trouve, 1, visited);
            if (trouve) break;
        }
    }
    if (!trouve) {
        printf("La trame n'a pas pu atteindre la station destination...\n");
    }
}

// --- Menu Display Function ---

void afficher_menu_principal() {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Lancer une simulation de trame (choix des stations)\n");
    printf("2. Afficher l'état des ports STP\n");
    printf("3. Afficher les tables MAC de tous les switches\n");
    printf("0. Quitter\n");
    printf("Votre choix : ");
}

// --- Main Function ---

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <fichier_config>\n", argv[0]);
        return 1;
    }

    reseau_t reseau;
    init_reseau(&reseau);

    if (charger_reseau_fichier(argv[1], &reseau) != 0) {
        printf("Erreur lors du chargement du réseau\n");
        return 1;
    }

    // Calculation and display of STP are done once at startup as in your original main
    printf("\n--- Calcul du Spanning Tree Protocol (STP) ---\n");
    // stp_calculer_spanning_tree expects a pointer, so use &reseau
    stp_calculer_spanning_tree(&reseau);
    printf("\n--- État initial des ports après STP ---\n");
    // stp_afficher_etat_ports expects a pointer, so use &reseau
    stp_afficher_etat_ports(&reseau);

    int choix_menu_principal = -1;

    while (choix_menu_principal != 0) {
        afficher_menu_principal();
        if (scanf("%d", &choix_menu_principal) != 1) {
            fprintf(stderr, "Entrée invalide. Veuillez entrer un nombre.\n");
            choix_menu_principal = -1; // Reset to loop again
            // Clear input buffer
            while (getchar() != '\n');
            continue;
        }
        getchar(); // Consume the newline character

        switch (choix_menu_principal) {
            case 1: { // Lancer une simulation de trame (choix des stations)
                int idx_src, idx_dest;

                printf("\n--- Lancement d'une simulation de trame --- \n");
                printf("Stations disponibles (index) : ");
                for (int i = 0; i < reseau.nb_equipements; i++) { // Access with '.'
                    if (reseau.equipements[i].type == STATION) { // Access with '.'
                        printf("%d ", i);
                    }
                }
                printf("\n");

                printf("Entrez l'index de la station source : ");
                if (scanf("%d", &idx_src) != 1) {
                    fprintf(stderr, "Entrée invalide pour l'index source.\n");
                    while (getchar() != '\n');
                    break;
                }
                getchar(); // Consume newline

                printf("Entrez l'index de la station destination : ");
                if (scanf("%d", &idx_dest) != 1) {
                    fprintf(stderr, "Entrée invalide pour l'index destination.\n");
                    while (getchar() != '\n');
                    break;
                }
                getchar(); // Consume newline

                // Validate inputs for existing stations
                if (idx_src < 0 || idx_src >= reseau.nb_equipements || reseau.equipements[idx_src].type != STATION) { // Access with '.'
                    printf("Erreur: L'index source %d n'est pas valide ou ne correspond pas à une station.\n", idx_src);
                } else if (idx_dest < 0 || idx_dest >= reseau.nb_equipements || reseau.equipements[idx_dest].type != STATION) { // Access with '.'
                    printf("Erreur: L'index destination %d n'est pas valide ou ne correspond pas à une station.\n", idx_dest);
                } else {
                    simuler_trame_station(&reseau, idx_src, idx_dest); // simuler_trame_station expects a pointer

                    printf("\n=== TABLES MAC APRÈS LA SIMULATION ===\n");
                    for (int i = 0; i < reseau.nb_equipements; i++) { // Access with '.'
                        if (reseau.equipements[i].type == SWITCH) { // Access with '.'
                            printf("\nSwitch %d :\n", i);
                            afficher_table_mac(&reseau.equipements[i].data.sw);
                        }
                    }
                }
                break;
            }
            case 2: // Afficher l'état des ports STP
                printf("\n=== État des ports STP ===\n");
                stp_afficher_etat_ports(&reseau); // stp_afficher_etat_ports expects a pointer
                break;
            case 3: // Afficher les tables MAC de tous les switches
                printf("\n=== TABLES MAC DE TOUS LES SWITCHES ===\n");
                for (int i = 0; i < reseau.nb_equipements; i++) { // Access with '.'
                    if (reseau.equipements[i].type == SWITCH) { // Access with '.'
                        printf("\nSwitch %d :\n", i);
                        afficher_table_mac(&reseau.equipements[i].data.sw);
                    }
                }
                break;
            case 0: // Quitter
                printf("Au revoir !\n");
                break;
            default:
                printf("Choix invalide ! Veuillez réessayer.\n");
                break;
        }
    }

    return EXIT_SUCCESS;
}