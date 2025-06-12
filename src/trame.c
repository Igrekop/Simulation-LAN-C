#include <stdio.h>
#include <stdint.h>
#include <string.h>   // Pour strlen, sscanf
#include <stdlib.h>   // Pour exit, ou strtol si besoin
#include "../include/reseau.h"
#include "../include/equipement.h"
#include "../include/trame.h"

// Déclaration anticipée de propager_trame
static void propager_trame(const trame *t, Equipement *sw, ReseauLocal *reseau, int src_id, PortManager *pm);

// Adresse MAC de broadcast (FF:FF:FF:FF:FF:FF)
const AdresseMAC ADRESSE_BROADCAST = 0xFFFFFFFFFFFF;

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
    (void)pm; // Pour éviter l'avertissement de paramètre non utilisé
    AdresseMAC mac = recevoir(t, station);
    if (mac != 0) {
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
    
    // Vérifier si le switch doit traiter la trame
    (void)recevoir(t, sw); // Pour éviter l'avertissement de variable non utilisée
    
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
    // Trouver l'index de l'équipement source
    int src_id = -1;
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if ((src->type == STATION && 
             memcmp(&reseau->equipements[i].typequipement.station.mac, &src->typequipement.station.mac, sizeof(AdresseMAC)) == 0) ||
            (src->type == SWITCH && 
             memcmp(&reseau->equipements[i].typequipement.sw.mac, &src->typequipement.sw.mac, sizeof(AdresseMAC)) == 0)) {
            src_id = i;
            break;
        }
    }

    if (src_id == -1) {
        printf("Erreur : équipement source non trouvé dans le réseau\n");
        return;
    }

    printf("\nDébut de la transmission depuis ");
    if (src->type == STATION) {
        printf("Station ");
        afficher_mac(src->typequipement.station.mac);
        printf(" (index %d)\n", src_id);
        
        // Debug : afficher les connexions de la station
        printf("Connexions de la station dans la matrice d'adjacence :\n");
        for (int i = 0; i < reseau->nb_equipements; i++) {
            if (reseau->matrice_adjacence[src_id][i] == 1) {
                printf("→ Connecté à l'équipement %d (", i);
                if (reseau->equipements[i].type == STATION) {
                    printf("Station ");
                    afficher_mac(reseau->equipements[i].typequipement.station.mac);
                } else {
                    printf("Switch ");
                    afficher_mac(reseau->equipements[i].typequipement.sw.mac);
                }
                printf(")\n");
            }
        }
    } else {
        printf("Switch ");
        afficher_mac(src->typequipement.sw.mac);
        printf("\n");
    }

    // Si c'est une station, trouver le switch connecté
    if (src->type == STATION) {
        int switch_trouve = 0;
        for (int i = 0; i < reseau->nb_equipements; i++) {
            if ((reseau->matrice_adjacence[src_id][i] == 1 || reseau->matrice_adjacence[src_id][i] == 0) && 
                reseau->equipements[i].type == SWITCH) {
                switch_trouve = 1;
                printf("→ Station connectée au Switch ");
                afficher_mac(reseau->equipements[i].typequipement.sw.mac);
                printf(" (poids du lien : %d)\n", reseau->matrice_adjacence[src_id][i]);
                
                // Traiter la trame dans le switch
                traiter_trame_switch(t, &reseau->equipements[i], pm, 0);
                
                // Propager la trame à partir du switch
                propager_trame(t, &reseau->equipements[i], reseau, src_id, pm);
                return;
            }
        }
        if (!switch_trouve) {
            printf("Erreur : station non connectée à un switch (vérifiez la matrice d'adjacence)\n");
            return;
        }
    }

    // Si c'est un switch, propager directement
    propager_trame(t, src, reseau, src_id, pm);
}

void propager_trame(const trame *t, Equipement *sw, ReseauLocal *reseau, int src_id, PortManager *pm) {
    int sw_id = -1;
    // Trouver l'index du switch
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if (memcmp(&reseau->equipements[i].typequipement.sw.mac, &sw->typequipement.sw.mac, sizeof(AdresseMAC)) == 0) {
            sw_id = i;
            break;
        }
    }

    if (sw_id == -1) return;

    // Chercher les connexions dans la matrice d'adjacence
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if ((reseau->matrice_adjacence[sw_id][i] == 1 || reseau->matrice_adjacence[sw_id][i] == 0) && i != src_id) {
            Equipement *dest = &reseau->equipements[i];
            
            // Si c'est la destination finale (station)
            if (dest->type == STATION && 
                memcmp(&dest->typequipement.station.mac, &t->dest_mac, sizeof(AdresseMAC)) == 0) {
                printf("→ Switch ");
                afficher_mac(sw->typequipement.sw.mac);
                printf(" transmet à la Station destination ");
                afficher_mac(dest->typequipement.station.mac);
                printf(" (poids du lien : %d)\n", reseau->matrice_adjacence[sw_id][i]);
                recevoir(t, dest);
                return;
            }
            
            // Si c'est un autre switch, continuer la propagation
            if (dest->type == SWITCH) {
                // Met à jour la table de commutation du switch traversé
                traiter_trame_switch(t, dest, pm, 0); // 0 ou le bon port selon votre logique
                printf("→ Switch ");
                afficher_mac(sw->typequipement.sw.mac);
                printf(" transmet au Switch ");
                afficher_mac(dest->typequipement.sw.mac);
                printf("\n");
                propager_trame(t, dest, reseau, sw_id, pm);
            }
        }
    }
}