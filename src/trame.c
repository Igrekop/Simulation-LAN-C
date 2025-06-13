#include <stdio.h>
#include <stdint.h>
#include <string.h>   // Pour strlen, sscanf
#include <stdlib.h>   // Pour exit, ou strtol si besoin
#include "reseau.h"
#include "equipement.h"
#include "trame.h"

// Définition de la structure pour identifier les trames
typedef struct {
    mac_addr_t source;
    mac_addr_t destination;
    uint32_t sequence;  // Pour identifier une trame unique
    mac_addr_t switch_id;  // Pour identifier quel switch a vu la trame
} TrameID;

// Table pour suivre les trames déjà vues
#define MAX_TRAMES_VUES 100
static TrameID trames_vues[MAX_TRAMES_VUES];
static int nb_trames_vues = 0;

// Prototypes des fonctions statiques
static int trouver_port_switch(reseau_t* reseau, int index_switch, int index_equipement);
static int trame_deja_vue(const ethernet_frame_t *t, mac_addr_t switch_id);
static void ajouter_trame_vue(const ethernet_frame_t *t, mac_addr_t switch_id);
static int trouver_equipement_par_mac(reseau_t* reseau, mac_addr_t mac);

void afficher_trame(const ethernet_frame_t *t) {
    printf("╔══════════════════════════════════════════╗\n");
    printf("║                TRAME ETHERNET            ║\n");
    printf("╠══════════════════════════════════════════╣\n");
    
    printf("║ MAC Source      : ");
    afficher_mac(t->src);
    printf("\n");

    printf("║ MAC Destination : ");
    afficher_mac(t->dest);
    printf("\n");

    // EtherType
    printf("╠══════════════════════════════════════════╣\n");
    printf("║ EtherType       : 0x%04X (", t->type);
    switch (t->type) {
        case 0x0800: printf("IPv4"); break;
        case 0x0806: printf("ARP");  break;
        case 0x86DD: printf("IPv6"); break;
        default:     printf("Inconnu");
    }
    printf(")\n");
    
    // Taille Données
    printf("╠══════════════════════════════════════════╣\n");
    printf("║ Taille données  : %zu octets\n", strlen((char*)t->data));
    printf("╚══════════════════════════════════════════╝\n");
}

void afficher_trame_complete(const ethernet_frame_t *t) {
    
    printf("╔══════════════════════════════════════════╗\n");
    printf("║        CONTENU BRUT DE LA TRAME          ║\n");
    printf("╠══════════════════════════════════════════╣\n");
    
  
    printf("║ Données brutes (%zu octets):\n", strlen((char*)t->data));
    
    const uint8_t *data = t->data;
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

mac_addr_t recevoir(const ethernet_frame_t *t, equipement_t *e, int port_entree, reseau_t* reseau) {
    // Vérifier si la trame a déjà été vue par ce switch spécifique
    if (e->type == SWITCH && trame_deja_vue(t, e->data.sw.mac)) {
        printf("⚠️ Trame déjà traitée par ce switch (MAC: ");
        afficher_mac(e->data.sw.mac);
        printf(")\n");
        return (mac_addr_t){{0}};
    }

    printf("╔══════════════════════════════════════════╗\n");
    printf("║               TRAME REÇUE                ║\n");
    printf("╠══════════════════════════════════════════╣\n");

    printf("║ Destination : ");
    afficher_mac(t->dest);
    printf("\n║ Récepteur   : ");

    if (e->type == STATION) {
        afficher_mac(e->data.station.mac);
        printf("\n╠══════════════════════════════════════════╣\n");
        
        if (mac_egal(t->dest, e->data.station.mac)) {
            printf("║ ➤ Trame acceptée : l'équipement est le destinataire.\n");
            printf("║ ➤ Message reçu : %s\n", (char*)t->data);
            printf("╚══════════════════════════════════════════╝\n");
            return e->data.station.mac;
        } else {
            printf("║ ➤ Trame ignorée : destinataire incorrect.\n");
            printf("╚══════════════════════════════════════════╝\n");
            return (mac_addr_t){{0}};
        }
    } else { // Cas du switch
        afficher_mac(e->data.sw.mac);
        printf("\n╠══════════════════════════════════════════╣\n");

        // 1. Apprentissage : mise à jour de la table de commutation
        if (port_entree >= 0 && port_entree < e->data.sw.nb_ports) {
            // Vérifier si cette adresse MAC est déjà apprise sur un autre port
            int port_existant = -1;
            for (int i = 0; i < e->data.sw.nb_ports; i++) {
                if (mac_egal(e->data.sw.mac_table[i], t->src)) {
                    port_existant = i;
                    break;
                }
            }
            
            // Si l'adresse MAC n'est pas déjà apprise, on l'apprend sur le port d'entrée
            if (port_existant == -1) {
                e->data.sw.mac_table[port_entree] = t->src;
                printf("║ ➤ Apprentissage : MAC source ");
                afficher_mac(t->src);
                printf(" apprise sur le port %d\n", port_entree);
            }
        }

        // 2. Vérifier si c'est la destination finale
        if (mac_egal(t->dest, e->data.sw.mac)) {
            printf("║ ➤ Trame acceptée : le switch est le destinataire.\n");
            printf("╚══════════════════════════════════════════╝\n");
            return e->data.sw.mac;
        }

        // 3. Chercher la destination dans la table de commutation
        int port_destination = -1;
        for (int i = 0; i < e->data.sw.nb_ports; i++) {
            if (mac_egal(e->data.sw.mac_table[i], t->dest)) {
                port_destination = i;
                break;
            }
        }

        if (port_destination != -1) {
            // La destination est connue, transfert unicast
            printf("║ ➤ Transfert unicast : MAC destination connue sur le port %d\n", port_destination);
            printf("╚══════════════════════════════════════════╝\n");
            // NE PAS ajouter la trame vue ici !
            for (int i = 0; i < reseau->nb_equipements; i++) {
                if (reseau->liens[port_destination].equip2 == i) {
                    // Ne pas rappeler le switch lui-même
                    if (&reseau->equipements[i] == e) continue;
                    return recevoir(t, &reseau->equipements[i], port_destination, reseau);
                }
            }
        } else {
            // La destination n'est pas connue, faire un broadcast
            printf("║ ➤ Diffusion : MAC destination inconnue\n");
            printf("╚══════════════════════════════════════════╝\n");
            printf("📢 Switch commence la diffusion sur les autres ports...\n");

            // AJOUTER la trame vue ici, pour éviter les boucles de broadcast
            if (e->type == SWITCH) {
                ajouter_trame_vue(t, e->data.sw.mac);
            }

            switch_t* sw = &e->data.sw;
            int index_self = trouver_equipement_par_mac(reseau, sw->mac);

            for (int port = 0; port < sw->nb_ports; port++) {
                if (port == port_entree) continue;

                int cible = sw->port_table[port];
                if (cible == -1 || cible == index_self) continue; // pas connecté ou lui-même

                if (reseau->equipements[cible].type == SWITCH &&
                    trame_deja_vue(t, reseau->equipements[cible].data.sw.mac)) {
                    printf("⚠️ Switch a déjà vu la trame, port %d ignoré\n", port);
                    continue; // évite rebouclage
                }

                printf("📤 Broadcast sur port %d vers équipement %d (%s)\n", port, cible,
                       reseau->equipements[cible].type == STATION ? "Station" : "Switch");
                mac_addr_t resultat = recevoir(t, &reseau->equipements[cible], port, reseau);
                if (mac_egal(resultat, t->dest)) {
                    printf("✅ Trame reçue par la station destination\n");
                }
            }
            return e->data.sw.mac;
        }
        return e->data.sw.mac;
    }
}

// Fonction pour trouver l'équipement par son adresse MAC
static int trouver_equipement_par_mac(reseau_t* reseau, mac_addr_t mac) {
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == STATION && 
            mac_egal(reseau->equipements[i].data.station.mac, mac)) {
            return i;
        }
        if (reseau->equipements[i].type == SWITCH && 
            mac_egal(reseau->equipements[i].data.sw.mac, mac)) {
            return i;
        }
    }
    return -1;
}

// Fonction pour trouver le port d'un switch connecté à un équipement
static int trouver_port_switch(reseau_t* reseau, int index_switch, int index_equipement) {
    // Si l'équipement est connecté au switch dans la matrice d'adjacence
    if (reseau->liens[index_equipement].equip2 == index_switch) {
        // On attribue le premier port disponible (0 à nb_ports-1)
        // On commence par 0 et on incrémente pour chaque nouvelle connexion
        int port = 0;
        while (port < reseau->equipements[index_switch].data.sw.nb_ports) {
            // Si le port est libre (pas d'adresse MAC associée)
            if (mac_egal(reseau->equipements[index_switch].data.sw.mac_table[port], (mac_addr_t){{0}})) {
                return port;
            }
            port++;
        }
    }
    return -1;
}

// Fonction pour vérifier si une trame a déjà été vue
static int trame_deja_vue(const ethernet_frame_t *t, mac_addr_t switch_id) {
    for (int i = 0; i < nb_trames_vues; i++) {
        if (mac_egal(trames_vues[i].source, t->src) && 
            mac_egal(trames_vues[i].destination, t->dest) &&
            trames_vues[i].sequence == t->fcs &&
            mac_egal(trames_vues[i].switch_id, switch_id)) {
            return 1;
        }
    }
    return 0;
}

// Fonction pour ajouter une trame à la liste des trames vues
static void ajouter_trame_vue(const ethernet_frame_t *t, mac_addr_t switch_id) {
    if (nb_trames_vues < MAX_TRAMES_VUES) {
        trames_vues[nb_trames_vues].source = t->src;
        trames_vues[nb_trames_vues].destination = t->dest;
        trames_vues[nb_trames_vues].sequence = t->fcs;
        trames_vues[nb_trames_vues].switch_id = switch_id;
        nb_trames_vues++;
    }
}

// Réinitialise la liste des trames vues

void envoyer_trame(const ethernet_frame_t *t, equipement_t *emetteur, mac_addr_t dest_mac, reseau_t* reseau) {
    // Générer un nouveau FCS unique pour chaque trame
    static uint32_t sequence_counter = 0;
    ethernet_frame_t t_copie = *t;  // Copie de la trame pour pouvoir modifier le FCS
    t_copie.fcs = sequence_counter++;

    // Initialiser le compteur de trames vues
    nb_trames_vues = 0;
    
    // Vérifier si la trame a déjà été vue
    if (trame_deja_vue(&t_copie, (mac_addr_t){{0}})) {  // 0 comme switch_id pour l'émetteur
        printf("⚠️ Trame déjà traitée, évitement de boucle\n");
        return;
    }
    
    // Ajouter la trame à la liste des trames vues
    ajouter_trame_vue(&t_copie, (mac_addr_t){{0}});  // 0 comme switch_id pour l'émetteur

    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║             ENVOI DE TRAME               ║\n");
    printf("╠══════════════════════════════════════════╣\n");
    printf("║ Émetteur : ");
    if (emetteur->type == STATION) {
        afficher_mac(emetteur->data.station.mac);
    } else {
        afficher_mac(emetteur->data.sw.mac);
    }
    printf("\n║ Destination : ");
    afficher_mac(dest_mac);
    printf("\n╚══════════════════════════════════════════╝\n");

    // Trouver l'index de l'émetteur dans le réseau
    int index_emetteur = trouver_equipement_par_mac(reseau, 
        (emetteur->type == STATION) ? emetteur->data.station.mac : emetteur->data.sw.mac);
    
    if (index_emetteur == -1) {
        printf("❌ Erreur : Émetteur non trouvé dans le réseau\n");
        return;
    }

    // Si l'émetteur est une station, on commence par le switch connecté
    if (emetteur->type == STATION) {
        // Trouver le switch connecté à cette station
        for (int i = 0; i < reseau->nb_equipements; i++) {
            if (reseau->equipements[i].type == SWITCH) {
                int port = trouver_port_switch(reseau, i, index_emetteur);
                if (port >= 0) {
                    printf("📤 Station envoie la trame au switch sur le port %d\n", port);
                    recevoir(&t_copie, &reseau->equipements[i], port, reseau);
                    return;
                }
            }
        }
        printf("❌ Erreur : Aucun switch connecté à la station\n");
        return;
    }

    // Si l'émetteur est un switch
    switch_t* sw = &emetteur->data.sw;
    
    // Vérifier si la MAC de destination est dans la table de commutation
    int port_destination = -1;
    for (int i = 0; i < sw->nb_ports; i++) {
        if (mac_egal(sw->mac_table[i], dest_mac)) {
            port_destination = i;
            break;
        }
    }

    if (port_destination != -1) {
        // MAC connue : transfert unicast
        printf("📤 Switch : MAC connue, transfert unicast sur le port %d\n", port_destination);
        
        // Trouver l'équipement connecté sur ce port
        for (int i = 0; i < reseau->nb_equipements; i++) {
            if (trouver_port_switch(reseau, index_emetteur, i) == port_destination) {
                // Si c'est la destination finale
                if (reseau->equipements[i].type == STATION && 
                    mac_egal(reseau->equipements[i].data.station.mac, dest_mac)) {
                    printf("✅ Trame arrivée à destination (station)\n");
                    // La station reçoit la trame
                    recevoir(&t_copie, &reseau->equipements[i], port_destination, reseau);
                    return;
                }
                // Si c'est un autre switch
                else if (reseau->equipements[i].type == SWITCH) {
                    printf("📤 Trame transmise à un autre switch\n");
                    recevoir(&t_copie, &reseau->equipements[i], port_destination, reseau);
                    return;
                }
            }
        }
    } else {
        // MAC inconnue : diffusion sur tous les ports sauf celui d'entrée
        printf("📢 Switch : MAC inconnue, diffusion sur tous les ports\n");
        
        // Tableau pour suivre les ports déjà utilisés
        int ports_utilises[MAX_PORTS] = {0};
        
        // Marquer le port d'entrée comme utilisé
        for (int i = 0; i < reseau->nb_equipements; i++) {
            int port = trouver_port_switch(reseau, index_emetteur, i);
            if (port >= 0) {
                ports_utilises[port] = 1;
                break;
            }
        }
        
        // Diffuser sur les ports non utilisés
        for (int i = 0; i < reseau->nb_equipements; i++) {
            int port = trouver_port_switch(reseau, index_emetteur, i);
            if (port >= 0 && !ports_utilises[port]) {
                printf("📤 Diffusion sur le port %d\n", port);
                ports_utilises[port] = 1;  // Marquer le port comme utilisé
                recevoir(&t_copie, &reseau->equipements[i], port, reseau);
            }
        }
    }
}

// Construction d'une trame Ethernet simple
void creer_trame_ethernet(
    ethernet_frame_t *trame,
    mac_addr_t src,
    mac_addr_t dest,
    uint16_t type,
    const uint8_t *data,
    uint16_t data_len
) {
    int i;
    for (i = 0; i < 7; i++) trame->preambule[i] = 0xAA;
    trame->sfd = 0xAB;
    trame->dest = dest;
    trame->src = src;
    trame->type = type;

    // Copie manuelle des données
    if (data_len > ETHERNET_MAX_DATA) data_len = ETHERNET_MAX_DATA;
    for (i = 0; i < data_len; i++) trame->data[i] = data[i];
    trame->data_len = data_len;

    // Padding si data < 46 octets
    int padding = (data_len < 46) ? (46 - data_len) : 0;
    for (i = 0; i < padding; i++) trame->bourrage[i] = 0;

    // FCS fictif
    trame->fcs = 0xDEADBEEF;
}

// Affiche une trame de façon lisible
void afficher_trame_utilisateur(const ethernet_frame_t *trame) {
    int i;
    printf("Trame Ethernet :\n");
    printf("  Destination : ");
    afficher_mac(trame->dest);
    printf("\n  Source      : ");
    afficher_mac(trame->src);
    printf("\n  Type        : 0x%04x\n", trame->type);
    printf("  Data length : %d octets\n", trame->data_len);
    printf("  Data        : ");
    for (i = 0; i < trame->data_len; i++) printf("%02x ", trame->data[i]);
    printf("\n  FCS         : 0x%08x\n", trame->fcs);
}


// Affiche la trame en hexadécimal (brut)
void afficher_trame_hex(const ethernet_frame_t *trame) {
    int i;
    printf("Trame (hex) :\n");
    for (i = 0; i < 7; i++) printf("%02x ", trame->preambule[i]);
    printf("%02x ", trame->sfd);
    for (i = 0; i < MAC_ADDR_LEN; i++) printf("%02x ", trame->dest.addr[i]);
    for (i = 0; i < MAC_ADDR_LEN; i++) printf("%02x ", trame->src.addr[i]);
    printf("%02x %02x ", (trame->type >> 8) & 0xFF, trame->type & 0xFF);
    for (i = 0; i < trame->data_len; i++) printf("%02x ", trame->data[i]);
    int bourrage_len = (trame->data_len < 46) ? (46 - trame->data_len) : 0;
    for (i = 0; i < bourrage_len; i++) printf("%02x ", trame->bourrage[i]);
    printf("%02x %02x %02x %02x\n",
        (trame->fcs >> 24) & 0xFF,
        (trame->fcs >> 16) & 0xFF,
        (trame->fcs >> 8) & 0xFF,
        trame->fcs & 0xFF);
}