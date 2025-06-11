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
                printf("\n=== TRAME ETHERNET DE TEST ===\n");
                trame t = {0}; 

   
                const char* mac_src_str = "AA:BB:CC:DD:EE:FF";
                const char* mac_dest_str = "01:45:23:a6:f7:ab";

                t.src_mac = convertir_en_mac(mac_src_str);
                t.dest_mac = convertir_en_mac(mac_dest_str);

                t.ethertype = 0x0800; 

                
                const char *message = "Bonjour je suis Yvann et je pue";
                size_t message_len = strlen(message);
                memcpy(t.DATA.contenu.data, message, message_len);
                t.DATA.contenu.data[message_len] = '\0'; 

                afficher_trame(&t);
                afficher_trame_complete(&t);
                
                recevoir(&t,&(mon_reseau.equipements[0]));
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
