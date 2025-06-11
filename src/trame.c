#include <stdio.h>
#include <stdint.h>
#include <string.h>   // Pour strlen, sscanf
#include <stdlib.h>   // Pour exit, ou strtol si besoin
#include "reseau.h"
#include "equipement.h"
#include "trame.h"

// Adresse MAC de broadcast (FF:FF:FF:FF:FF:FF)
const AdresseMAC ADRESSE_BROADCAST = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void afficher_trame(const trame *t) {
    printf("╔══════════════════════════════════════════╗\n");
    printf("║                TRAME ETHERNET            ║\n");
    printf("╠══════════════════════════════════════════╣\n");
    
    printf("║ MAC Source      : ");
    afficher_mac(t->src_mac);
    printf("\n");

    printf("║ MAC Destination : ");
    afficher_mac(t->dest_mac);
    printf("\n");

    // EtherType
    printf("╠══════════════════════════════════════════╣\n");
    printf("║ EtherType       : 0x%04X (", t->ethertype);
    switch (t->ethertype) {
        case 0x0800: printf("IPv4"); break;
        case 0x0806: printf("ARP");  break;
        case 0x86DD: printf("IPv6"); break;
        default:     printf("Inconnu");
    }
    printf(")\n");
    
    // Taille Données
    printf("╠══════════════════════════════════════════╣\n");
    printf("║ Taille données  : %zu octets\n", strlen((char*)t->DATA.contenu.data));
    printf("╚══════════════════════════════════════════╝\n");
}

void afficher_trame_complete(const trame *t) {
    
    printf("╔══════════════════════════════════════════╗\n");
    printf("║        CONTENU BRUT DE LA TRAME          ║\n");
    printf("╠══════════════════════════════════════════╣\n");
    
  
    printf("║ Données brutes (%zu octets):\n", strlen((char*)t->DATA.contenu.data));
    
    const uint8_t *data = t->DATA.contenu.data;
    size_t data_size = strlen((char*)data);
    size_t line_count = 0;
    
    for(size_t i = 0; i < data_size; i++) {
        if(i % 16 == 0) {
            if(i != 0) printf("\n");  
            printf("║ %04zX: ", line_count++ * 16);  
        }
        
    
        printf("%02X ", data[i]);
        

        if(i % 8 == 7) printf(" ");
    }
    
    printf("\n╚══════════════════════════════════════════╝\n");
    

    printf("\n %zu octets de données\n", 
           data_size);
}

AdresseMAC recevoir(const trame *t, Equipement *e) {
    printf("╔══════════════════════════════════════════╗\n");
    printf("║               TRAME REÇUE                ║\n");
    printf("╠══════════════════════════════════════════╣\n");

    printf("║ Destination : ");
    afficher_mac(t->dest_mac);
    printf("\n║ Récepteur   : ");
    if (e->type == STATION) {
    afficher_mac(e->typequipement.station.mac);
    printf("\n╠══════════════════════════════════════════╣\n");

    
    if (t->dest_mac == e->typequipement.station.mac) {
        printf("║ ➤ Trame acceptée : l'équipement est le destinataire.\n");
        printf("╚══════════════════════════════════════════╝\n");
        return e->typequipement.station.mac;
    } else {
        printf("║ ➤ Trame ignorée : destinataire incorrect.\n");
        printf("╚══════════════════════════════════════════╝\n");
        return 0;  
    }
    } 
    else {
    afficher_mac(e->typequipement.sw.mac);

    printf("\n╠══════════════════════════════════════════╣\n");

    
    if (t->dest_mac == e->typequipement.sw.mac) {
        printf("║ ➤ Trame acceptée : l'équipement est le destinataire.\n");
        printf("╚══════════════════════════════════════════╝\n");
        return e->typequipement.sw.mac;
    } else {
        printf("║ ➤ Trame ignorée : destinataire incorrect.\n");
        printf("╚══════════════════════════════════════════╝\n");
        return 0;  
    }
    }
}

void initialiser_trame(trame *t, const AdresseMAC *src, const AdresseMAC *dest) {
    // Initialiser le préambule (alternance de 1 et 0)
    for (int i = 0; i < 7; i++) {
        t->preambule[i] = 0xAA;
    }
    t->SFD = 0xAB;  // Start Frame Delimiter
    
    // Copier les adresses MAC
    memcpy(&t->src_mac, src, sizeof(AdresseMAC));
    memcpy(&t->dest_mac, dest, sizeof(AdresseMAC));
    
    // Initialiser le reste de la trame
    t->ethertype = 0x0800;  // IPv4
    memset(&t->DATA, 0, sizeof(t->DATA));
    t->FCS = 0;  // À calculer en pratique
}

int est_broadcast(const AdresseMAC *mac) {
    return memcmp(mac, &ADRESSE_BROADCAST, sizeof(AdresseMAC)) == 0;
}

void traiter_trame_station(const trame *t, Equipement *station, PortManager *pm) {
    AdresseMAC mac = recevoir(t, station);
    if (mac != 0) {  // Si la trame a été acceptée
        printf("Station ");
        afficher_mac(station->typequipement.station.mac);
        printf(" traite la trame\n");
    }
}

void traiter_trame_switch(const trame *t, Equipement *sw, PortManager *pm, int port_reception) {
    // Mettre à jour la table de commutation avec l'adresse source
    mettre_a_jour_table_commutation(&sw->typequipement.sw, &t->src_mac, port_reception);
    
    // Mettre à jour le port avec la dernière adresse MAC source vue
    pm->ports[port_reception].last_src_mac = t->src_mac;
    
    // Utiliser recevoir() pour vérifier si le switch doit traiter la trame
    AdresseMAC mac = recevoir(t, sw);
    
    // Chercher le port de destination dans la table de commutation
    int port_dest = -1;
    if (!est_broadcast(&t->dest_mac)) {
        for (int i = 0; i < MAX_PORTS; i++) {
            if (memcmp(&sw->typequipement.sw.table_commutation[i], &t->dest_mac, sizeof(AdresseMAC)) == 0) {
                port_dest = i;
                break;
            }
        }
    }
    
    if (port_dest != -1) {
        printf("Switch ");
        afficher_mac(sw->typequipement.sw.mac);
        printf(" transmet la trame vers le port %d\n", port_dest);
    } else {
        printf("Switch ");
        afficher_mac(sw->typequipement.sw.mac);
        printf(" fait un broadcast de la trame\n");
    }
}

void envoyer_trame(const trame *t, Equipement *src, PortManager *pm, ReseauLocal *reseau) {
    // Trouver l'équipement connecté au port de la station source
    int port_src = 0;  // On suppose que la station utilise le port 0
    if (pm->ports[port_src].status == PORT_CONNECTED) {
        int equip_dest = pm->ports[port_src].connected_to_equip;
        int port_dest = pm->ports[port_src].connected_to_port;
        
        // Traiter la trame selon le type d'équipement de destination
        if (reseau->equipements[equip_dest].type == STATION) {
            traiter_trame_station(t, &reseau->equipements[equip_dest], pm);
        } else if (reseau->equipements[equip_dest].type == SWITCH) {
            traiter_trame_switch(t, &reseau->equipements[equip_dest], pm, port_dest);
        }
    }
}