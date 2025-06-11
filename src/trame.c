    #include <stdio.h>
    #include <stdint.h>
    #include <string.h>   // Pour strlen, sscanf
    #include <stdlib.h>   // Pour exit, ou strtol si besoin
    #include "reseau.h"
    #include "equipement.h"
    #include "trame.h"
    
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