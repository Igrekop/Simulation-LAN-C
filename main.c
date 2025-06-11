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
                uint8_t mac_src[6]  = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
                uint8_t mac_dest[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
                
                memcpy(t.src_mac, mac_src, 6);
                memcpy(t.dest_mac, mac_dest, 6);
                t.ethertype = 0x0800;

                const char *message = "Bonjour je suis Yvann et je pue";
                size_t message_len = strlen(message);
                memcpy(t.DATA.contenu.data, message, message_len);

                afficher_trame(&t);
                printf("\nAffichage détaillé de la trame :\n");
                afficher_trame_complete(&t);
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
>>>>>>> 623154fc6231aafb5158edce79f01ec3ac7bb392
