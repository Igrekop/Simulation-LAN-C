#include <stdio.h>
#include "equipement.h"
#include "reseau.h"
#include "trame.h"
#include <stdlib.h>
#include <string.h>


int main() {

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
    // Affichage du réseau
    

    // Partie 2
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
    
       printf("\nCréation et affichage d'une trame (test)\n");

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
    
    recevoir(&t,&(r.equipements[0]));



    
    return EXIT_SUCCESS;
    

}
