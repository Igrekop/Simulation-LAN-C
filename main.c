#include <stdio.h>
#include "equipement.h"
#include "reseau.h"
#include "trame.h"
#include <stdlib.h>
#include <string.h>

void afficher_menu_principal() {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Afficher les machines\n");
    printf("2. Afficher la matrice d'adjacence\n");
    printf("3. Partie trame ethernet\n");
    printf("0. Quitter\n");
    printf("Votre choix : ");
}

void afficher_sous_menu_machines() {
    printf("\n=== SOUS-MENU MACHINES ===\n");
    printf("1. Afficher les switches\n");
    printf("2. Afficher les stations\n");
    printf("3. Afficher tous les équipements\n");
    printf("0. Retour au menu principal\n");
    printf("Votre choix : ");
}

void menu_trames_ethernet(ReseauLocal *reseau) {
    int choix = 0;
    trame t;
    PortManager pm;
    pm.nb_ports = MAX_PORTS;
    
    // Initialiser tous les ports
    for (int i = 0; i < MAX_PORTS; i++) {
        initialiser_port(&pm.ports[i], i);
    }

    while (choix != 3) {
        printf("\n=== MENU TRAMES ETHERNET ===\n");
        printf("1. Envoyer une trame\n");
        printf("2. Afficher les tables de commutation\n");
        printf("3. Retour au menu principal\n");
        printf("Votre choix : ");
        scanf("%d", &choix);
        getchar(); // Consommer le \n

        switch (choix) {
            case 1: {
                int src_id, dest_id;
                printf("\nListe des équipements disponibles :\n");
                for (int i = 0; i < reseau->nb_equipements; i++) {
                    printf("%d. ", i);
                    if (reseau->equipements[i].type == STATION) {
                        printf("Station ");
                        afficher_mac(reseau->equipements[i].typequipement.station.mac);
                    } else {
                        printf("Switch ");
                        afficher_mac(reseau->equipements[i].typequipement.sw.mac);
                    }
                    printf("\n");
                }

                printf("\nChoisir l'équipement source (0-%d) : ", reseau->nb_equipements - 1);
                scanf("%d", &src_id);
                printf("Choisir l'équipement destination (0-%d) : ", reseau->nb_equipements - 1);
                scanf("%d", &dest_id);
                getchar(); // Consommer le \n

                if (src_id >= 0 && src_id < reseau->nb_equipements &&
                    dest_id >= 0 && dest_id < reseau->nb_equipements) {
                    
                    // Préparer la trame
                    AdresseMAC src_mac, dest_mac;
                    if (reseau->equipements[src_id].type == STATION) {
                        src_mac = reseau->equipements[src_id].typequipement.station.mac;
                    } else {
                        src_mac = reseau->equipements[src_id].typequipement.sw.mac;
                    }
                    
                    if (reseau->equipements[dest_id].type == STATION) {
                        dest_mac = reseau->equipements[dest_id].typequipement.station.mac;
                    } else {
                        dest_mac = reseau->equipements[dest_id].typequipement.sw.mac;
                    }

                    initialiser_trame(&t, &src_mac, &dest_mac);
                    printf("\nTrame initialisée :\n");
                    afficher_trame(&t);
                    
                    // Envoyer la trame
                    envoyer_trame(&t, &reseau->equipements[src_id], &pm, reseau);
                } else {
                    printf("Erreur : indices d'équipements invalides\n");
                }
                break;
            }
            case 2: {
                printf("\nTables de commutation des switches :\n");
                for (int i = 0; i < reseau->nb_equipements; i++) {
                    if (reseau->equipements[i].type == SWITCH) {
                        printf("\nSwitch ");
                        afficher_mac(reseau->equipements[i].typequipement.sw.mac);
                        printf(" :\n");
                        for (int j = 0; j < MAX_PORTS; j++) {
                            if (reseau->equipements[i].typequipement.sw.table_commutation[j] != 0) {
                                printf("Port %d -> MAC : ", j);
                                afficher_mac(reseau->equipements[i].typequipement.sw.table_commutation[j]);
                                printf("\n");
                            }
                        }
                    }
                }
                break;
            }
            case 3:
                printf("Retour au menu principal...\n");
                break;
            default:
                printf("Choix invalide\n");
        }
    }
}

int main() {
    ReseauLocal mon_reseau;
    mon_reseau.nb_equipements = 0;

    printf("Chargement du réseau depuis le fichier 'reseau_config.txt'...\n");
    if (charger_reseau_fichier("reseau_config.txt", &mon_reseau) != 0) {
        fprintf(stderr, "Échec du chargement du réseau.\n");
        return EXIT_FAILURE;
    }
    printf("Réseau chargé avec succès !\n");

    int choix = -1;
    int sous_choix = -1;

    while (choix != 0) {
        afficher_menu_principal();
        scanf("%d", &choix);
        getchar(); // Consomme le retour à la ligne

        switch (choix) {
            case 1:
                while (sous_choix != 0) {
                    afficher_sous_menu_machines();
                    scanf("%d", &sous_choix);
                    getchar();

                    switch (sous_choix) {
                        case 1:
                            printf("\n=== SWITCHES ===\n");
                            for (int i = 0; i < mon_reseau.nb_equipements; i++) {
                                if (mon_reseau.equipements[i].type == SWITCH) {
                                    printf("\nSwitch %d :\n", i + 1);
                                    afficher_switch(mon_reseau.equipements[i].typequipement.sw);
                                }
                            }
                            break;
                        case 2:
                            printf("\n=== STATIONS ===\n");
                            for (int i = 0; i < mon_reseau.nb_equipements; i++) {
                                if (mon_reseau.equipements[i].type == STATION) {
                                    printf("\nStation %d :\n", i + 1);
                                    afficher_station(mon_reseau.equipements[i].typequipement.station);
                                }
                            }
                            break;
                        case 3:
                            afficher_reseau(&mon_reseau);
                            break;
                        case 0:
                            break;
                        default:
                            printf("Choix invalide !\n");
                    }
                }
                sous_choix = -1;
                break;

            case 2:
                printf("\n=== MATRICE D'ADJACENCE ===\n");
                afficher_matrice_adjacence(&mon_reseau);
                break;

            case 3:
                menu_trames_ethernet(&mon_reseau);
                break;

            case 0:
                printf("Au revoir !\n");
                break;

            default:
                printf("Choix invalide !\n");
        }
    }
    return EXIT_SUCCESS;
}
