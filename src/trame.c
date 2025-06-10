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
    
    // MAC
    printf("║ MAC Source      : ");
    for (int i = 0; i < 6; i++) {
        printf("%02X", t->src_mac[i]);
        if (i < 5) printf(":");
    }
    printf("\n");

    printf("║ MAC Destination : ");
    for (int i = 0; i < 6; i++) {
        printf("%02X", t->dest_mac[i]);
        if (i < 5) printf(":");
    }
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

       
       
