#include <stdio.h>
#include "equipement.h"
#include "reseau.h"
#include "trame.h"
#include <stdlib.h>
#include <string.h>


int main() {

    /*printf("Partie 1 : Création et affichage d'un système réseau\n");
    printf("\n");

    // Partie 1
    ReseauLocal r = {0};
    r.nb_equipements = 2;

    // Ajout d’un switch
    r.equipements[0].type = SWITCH;
    r.equipements[0].typequipement.sw.mac = 0x014523a6f7ab;
    r.equipements[0].typequipement.sw.nb_ports = 2;
    r.equipements[0].typequipement.sw.priorite = 1024;
    r.equipements[0].typequipement.sw.table_commutation[0] = 0x001122334455;

    // Ajout d’une station
    r.equipements[1].type = STATION;
    r.equipements[1].typequipement.station.mac = 0x54d6a682c523;
    r.equipements[1].typequipement.station.ip = (130 << 24) | (79 << 16) | (80 << 8) | 21; //donnera 130.79.80.21 (<<24 1er octet etc)

    // Lien entre eux
    r.matrice_adjacence[0][1] = 4;
    r.matrice_adjacence[1][0] = 4;

    afficher_reseau(&r);
    //return 0; // --> en commentaire pour pouvoir continuer
    // Affichage du réseau*/
    

    printf("\nPartie 2 : Lire et afficher un système à partir d'un fichier annexe\n");
    printf(" \n");

    ReseauLocal mon_reseau; // Déclare une structure ReseauLocal
    
    // Initialisation de base au cas où le chargement échoue
    mon_reseau.nb_equipements = 0;
    // La matrice d'adjacence sera initialisée par charger_reseau_depuis_fichier

    printf("Chargement du réseau depuis le fichier 'reseau_config.txt'...\n\n");
    
    // Appel de la fonction de chargement du réseau
    if (charger_reseau_fichier("reseau_config.txt", &mon_reseau) != 0) {
        fprintf(stderr, "Échec du chargement du réseau.\n");
        return EXIT_FAILURE; // Quitte le programme avec un code d'erreur
    }
    
    printf("Réseau chargé avec succès !\n\n");
    
    // Affichage du réseau chargé
    afficher_reseau(&mon_reseau);
    
    printf("\n Partie 3 : Création et affichage d'une trame(test)\n");

    trame t = {0}; // Initialisation à 0

    // Remplissage des champs
    uint8_t mac_src[6]  = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    uint8_t mac_dest[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    
    memcpy(t.src_mac, mac_src, 6);
    memcpy(t.dest_mac, mac_dest, 6);

    t.ethertype = 0x0800; // IPv4

    // Données exemple
    const char *message = "Bonjour je suis Yvann et je pue";
    size_t message_len = strlen(message);
    memcpy(t.DATA.contenu.data, message, message_len);

    // Appel de la fonction d'affichage
    afficher_trame(&t);
    
    afficher_trame_complete(&t);

    
    return EXIT_SUCCESS;
    

}
