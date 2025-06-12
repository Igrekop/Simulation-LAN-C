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

void afficher_menu_trames() {
    printf("\n=== MENU TRAMES ETHERNET ===\n");
    printf("1. Envoyer une trame\n");
    printf("2. Afficher les tables de commutation\n");
    printf("3. Envoyer une trame de test\n");
    printf("0. Retour au menu principal\n");
    printf("Votre choix : ");
}

int afficher_stations_disponibles(ReseauLocal* reseau) {
    printf("\nStations disponibles :\n");
    int nb_stations = 0;
    int indices_stations[100]; // Tableau pour stocker les indices des stations
    
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == STATION) {
            printf("%d. Station (MAC: ", nb_stations + 1);
            afficher_mac(reseau->equipements[i].typequipement.station.mac);
            printf(", IP: ");
            afficher_ip(reseau->equipements[i].typequipement.station.ip);
            printf(")\n");
            indices_stations[nb_stations] = i;
            nb_stations++;
        }
    }
    
    if (nb_stations == 0) {
        printf("Aucune station disponible dans le réseau.\n");
        return -1;
    }
    
    int choix;
    printf("\nChoisissez une station (1-%d) : ", nb_stations);
    scanf("%d", &choix);
    getchar();
    
    if (choix < 1 || choix > nb_stations) {
        printf("❌ Choix invalide !\n");
        return -1;
    }
    
    return indices_stations[choix - 1];
}

void afficher_tables_commutation(ReseauLocal* reseau) {
    printf("\n=== TABLES DE COMMUTATION ===\n");
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == SWITCH) {
            printf("\nSwitch %d (MAC: ", i + 1);
            afficher_mac(reseau->equipements[i].typequipement.sw.mac);
            printf(")\n");
            printf("Table de commutation :\n");
            
            Switch* sw = &reseau->equipements[i].typequipement.sw;
            int entree_trouvee = 0;
            
            for (int port = 0; port < sw->nb_ports; port++) {
                if (sw->table_commutation[port] != 0) {
                    printf("  Port %d → MAC: ", port);
                    afficher_mac(sw->table_commutation[port]);
                    printf("\n");
                    entree_trouvee = 1;
                }
            }
            
            if (!entree_trouvee) {
                printf("  Table vide (aucune entrée apprise)\n");
            }
        }
    }
}

void envoyer_trame_interactive(ReseauLocal* reseau) {
    trame t = {0};
    int index_emetteur, index_destinataire;
    
    // Sélection de l'émetteur
    printf("\n=== Sélection de l'émetteur ===\n");
    index_emetteur = afficher_stations_disponibles(reseau);
    if (index_emetteur == -1) return;
    
    // Sélection du destinataire
    printf("\n=== Sélection du destinataire ===\n");
    index_destinataire = afficher_stations_disponibles(reseau);
    if (index_destinataire == -1) return;
    
    if (index_emetteur == index_destinataire) {
        printf("❌ Erreur : L'émetteur et le destinataire sont identiques\n");
        return;
    }
    
    // Préparation de la trame
    t.src_mac = reseau->equipements[index_emetteur].typequipement.station.mac;
    t.dest_mac = reseau->equipements[index_destinataire].typequipement.station.mac;
    t.ethertype = 0x0800; // IPv4
    
    // Message de test
    char message[100];
    printf("\nEntrez le message à envoyer : ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0'; // Supprime le retour à la ligne
    
    size_t message_len = strlen(message);
    memcpy(t.DATA.contenu.data, message, message_len);
    t.DATA.contenu.data[message_len] = '\0';
    
    // Affichage de la trame
    printf("\nTrame à envoyer :\n");
    afficher_trame(&t);
    
    // Envoi de la trame
    printf("\nEnvoi de la trame...\n");
    envoyer_trame(&t, &reseau->equipements[index_emetteur], t.dest_mac, reseau);
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
    int choix_trames = -1;

    while (choix != 0) {
        afficher_menu_principal();
        scanf("%d", &choix);
        getchar();

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
                while (choix_trames != 0) {
                    afficher_menu_trames();
                    scanf("%d", &choix_trames);
                    getchar();

                    switch (choix_trames) {
                        case 1:
                            envoyer_trame_interactive(&mon_reseau);
                            break;
                            
                        case 2:
                            afficher_tables_commutation(&mon_reseau);
                            break;
                            
                        case 3:
                            printf("\n=== TRAME ETHERNET DE TEST ===\n");
                            trame t = {0};
                            
                            const char* mac_src_str = "AA:BB:CC:DD:EE:FF";
                            const char* mac_dest_str = "01:45:23:a6:f7:ab";
                            
                            t.src_mac = convertir_en_mac(mac_src_str);
                            t.dest_mac = convertir_en_mac(mac_dest_str);
                            t.ethertype = 0x0800;
                            
                            const char *message = "Message de test";
                            size_t message_len = strlen(message);
                            memcpy(t.DATA.contenu.data, message, message_len);
                            t.DATA.contenu.data[message_len] = '\0';
                            
                            afficher_trame(&t);
                            afficher_trame_complete(&t);
                            
                            // Trouver une station pour envoyer la trame
                            for (int i = 0; i < mon_reseau.nb_equipements; i++) {
                                if (mon_reseau.equipements[i].type == STATION) {
                                    envoyer_trame(&t, &mon_reseau.equipements[i], t.dest_mac, &mon_reseau);
                                    break;
                                }
                            }
                            break;
                            
                        case 0:
                            break;
                            
                        default:
                            printf("Choix invalide !\n");
                    }
                }
                choix_trames = -1;
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
