#include <stdio.h>
    #include <stdint.h>
    #include <string.h>   // Pour strlen, sscanf
    #include <stdlib.h>   // Pour exit, ou strtol si besoin
    #include "reseau.h"
    #include "equipement.h"
#include "../include/trame.h"

// Définition de la structure pour identifier les trames
typedef struct {
    AdresseMAC source;
    AdresseMAC destination;
    uint32_t sequence;  // Pour identifier une trame unique
    AdresseMAC switch_id;  // Pour identifier quel switch a vu la trame
} TrameID;

// Table pour suivre les trames déjà vues
#define MAX_TRAMES_VUES 100
static TrameID trames_vues[MAX_TRAMES_VUES];
static int nb_trames_vues = 0;

// Prototypes des fonctions statiques
static int trouver_port_switch(ReseauLocal* reseau, int index_switch, int index_equipement);
static int trame_deja_vue(const trame *t, AdresseMAC switch_id);
static void ajouter_trame_vue(const trame *t, AdresseMAC switch_id);
    
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

AdresseMAC recevoir(const trame *t, Equipement *e, int port_entree, ReseauLocal* reseau) {
    // Vérifier si la trame a déjà été vue par ce switch spécifique
    if (e->type == SWITCH && trame_deja_vue(t, e->typequipement.sw.mac)) {
        printf("⚠️ Trame déjà traitée par ce switch (MAC: ");
        afficher_mac(e->typequipement.sw.mac);
        printf(")\n");
        return 0;
    }
    
    // Ajouter la trame à la liste des trames vues pour ce switch
    if (e->type == SWITCH) {
        ajouter_trame_vue(t, e->typequipement.sw.mac);
    }

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
            printf("║ ➤ Message reçu : %s\n", (char*)t->DATA.contenu.data);
            printf("╚══════════════════════════════════════════╝\n");
            return e->typequipement.station.mac;
        } else {
            printf("║ ➤ Trame ignorée : destinataire incorrect.\n");
            printf("╚══════════════════════════════════════════╝\n");
            return 0;
        }
    } else { // Cas du switch
        afficher_mac(e->typequipement.sw.mac);
        printf("\n╠══════════════════════════════════════════╣\n");

        // 1. Apprentissage : mise à jour de la table de commutation
        if (port_entree >= 0 && port_entree < e->typequipement.sw.nb_ports) {
            // Vérifier si cette adresse MAC est déjà apprise sur un autre port
            int port_existant = -1;
            for (int i = 0; i < e->typequipement.sw.nb_ports; i++) {
                if (e->typequipement.sw.table_commutation[i] == t->src_mac) {
                    port_existant = i;
                    break;
                }
            }
            
            // Si l'adresse MAC n'est pas déjà apprise, on l'apprend sur le port d'entrée
            if (port_existant == -1) {
                e->typequipement.sw.table_commutation[port_entree] = t->src_mac;
                printf("║ ➤ Apprentissage : MAC source ");
                afficher_mac(t->src_mac);
                printf(" apprise sur le port %d\n", port_entree);
            }
        }

        // 2. Vérifier si c'est la destination finale
        if (t->dest_mac == e->typequipement.sw.mac) {
            printf("║ ➤ Trame acceptée : le switch est le destinataire.\n");
            printf("╚══════════════════════════════════════════╝\n");
            return e->typequipement.sw.mac;
        }

        // 3. Chercher la destination dans la table de commutation
        int port_destination = -1;
        for (int i = 0; i < e->typequipement.sw.nb_ports; i++) {
            if (e->typequipement.sw.table_commutation[i] == t->dest_mac) {
                port_destination = i;
                break;
            }
        }

        if (port_destination != -1) {
            // La destination est connue, transfert unicast
            printf("║ ➤ Transfert unicast : MAC destination connue sur le port %d\n", port_destination);
            printf("╚══════════════════════════════════════════╝\n");
            
            // Trouver l'équipement connecté sur ce port
            for (int i = 0; i < reseau->nb_equipements; i++) {
                if (reseau->matrice_adjacence[port_destination][i] > 0) {
                    return recevoir(t, &reseau->equipements[i], port_destination, reseau);
                }
            }
        } else {
            // La destination n'est pas connue, faire un broadcast
            printf("║ ➤ Diffusion : MAC destination inconnue\n");
            printf("╚══════════════════════════════════════════╝\n");
            
            // Diffuser sur tous les ports sauf celui d'entrée
            printf("📢 Switch commence la diffusion sur les autres ports...\n");
            for (int port = 0; port < e->typequipement.sw.nb_ports; port++) {
                if (port != port_entree) {
                    // Trouver l'équipement connecté sur ce port
                    for (int i = 0; i < reseau->nb_equipements; i++) {
                        if (reseau->matrice_adjacence[port][i] > 0) {
                            // Ne pas envoyer à l'émetteur
                            if (reseau->equipements[i].type == STATION && 
                                reseau->equipements[i].typequipement.station.mac == t->src_mac) {
                                continue;
                            }
                            
                            printf("📤 Broadcast sur le port %d vers l'équipement %d (%s)\n", 
                                   port, i, 
                                   reseau->equipements[i].type == STATION ? "Station" : "Switch");
                            
                            // Pour les switches, vérifier si on a déjà reçu la trame
                            if (reseau->equipements[i].type == SWITCH) {
                                if (trame_deja_vue(t, reseau->equipements[i].typequipement.sw.mac)) {
                                    printf("⚠️ Switch a déjà vu la trame, port %d ignoré\n", port);
                                    continue;
                                }
                            }
                            
                            AdresseMAC resultat = recevoir(t, &reseau->equipements[i], port, reseau);
                            if (resultat == t->dest_mac) {
                                printf("✅ Trame reçue par la station destination\n");
                            }
                        }
                    }
                }
            }
        }
        return e->typequipement.sw.mac;
    }
}

// Fonction pour trouver l'équipement par son adresse MAC
static int trouver_equipement_par_mac(ReseauLocal* reseau, AdresseMAC mac) {
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == STATION && 
            reseau->equipements[i].typequipement.station.mac == mac) {
            return i;
        }
        if (reseau->equipements[i].type == SWITCH && 
            reseau->equipements[i].typequipement.sw.mac == mac) {
            return i;
        }
    }
    return -1;
}

// Fonction pour trouver le port d'un switch connecté à un équipement
static int trouver_port_switch(ReseauLocal* reseau, int index_switch, int index_equipement) {
    // Si l'équipement est connecté au switch dans la matrice d'adjacence
    if (reseau->matrice_adjacence[index_equipement][index_switch] > 0) {
        // On attribue le premier port disponible (0 à nb_ports-1)
        // On commence par 0 et on incrémente pour chaque nouvelle connexion
        int port = 0;
        while (port < reseau->equipements[index_switch].typequipement.sw.nb_ports) {
            // Si le port est libre (pas d'adresse MAC associée)
            if (reseau->equipements[index_switch].typequipement.sw.table_commutation[port] == 0) {
                return port;
            }
            port++;
        }
    }
    return -1;
}

// Fonction pour vérifier si une trame a déjà été vue
static int trame_deja_vue(const trame *t, AdresseMAC switch_id) {
    for (int i = 0; i < nb_trames_vues; i++) {
        if (trames_vues[i].source == t->src_mac && 
            trames_vues[i].destination == t->dest_mac &&
            trames_vues[i].sequence == t->FCS &&
            trames_vues[i].switch_id == switch_id) {
            return 1;
        }
    }
    return 0;
}

// Fonction pour ajouter une trame à la liste des trames vues
static void ajouter_trame_vue(const trame *t, AdresseMAC switch_id) {
    if (nb_trames_vues < MAX_TRAMES_VUES) {
        trames_vues[nb_trames_vues].source = t->src_mac;
        trames_vues[nb_trames_vues].destination = t->dest_mac;
        trames_vues[nb_trames_vues].sequence = t->FCS;
        trames_vues[nb_trames_vues].switch_id = switch_id;
        nb_trames_vues++;
    }
}

void envoyer_trame(const trame *t, Equipement *emetteur, AdresseMAC dest_mac, ReseauLocal* reseau) {
    // Générer un nouveau FCS unique pour chaque trame
    static uint32_t sequence_counter = 0;
    trame t_copie = *t;  // Copie de la trame pour pouvoir modifier le FCS
    t_copie.FCS = sequence_counter++;
    
    // Vérifier si la trame a déjà été vue
    if (trame_deja_vue(&t_copie, 0)) {  // 0 comme switch_id pour l'émetteur
        printf("⚠️ Trame déjà traitée, évitement de boucle\n");
        return;
    }
    
    // Ajouter la trame à la liste des trames vues
    ajouter_trame_vue(&t_copie, 0);  // 0 comme switch_id pour l'émetteur

    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║             ENVOI DE TRAME               ║\n");
    printf("╠══════════════════════════════════════════╣\n");
    printf("║ Émetteur : ");
    if (emetteur->type == STATION) {
        afficher_mac(emetteur->typequipement.station.mac);
    } else {
        afficher_mac(emetteur->typequipement.sw.mac);
    }
    printf("\n║ Destination : ");
    afficher_mac(dest_mac);
    printf("\n╚══════════════════════════════════════════╝\n");

    // Trouver l'index de l'émetteur dans le réseau
    int index_emetteur = trouver_equipement_par_mac(reseau, 
        (emetteur->type == STATION) ? emetteur->typequipement.station.mac : emetteur->typequipement.sw.mac);
    
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
    Switch* sw = &emetteur->typequipement.sw;
    
    // Vérifier si la MAC de destination est dans la table de commutation
    int port_destination = -1;
    for (int i = 0; i < sw->nb_ports; i++) {
        if (sw->table_commutation[i] == dest_mac) {
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
                    reseau->equipements[i].typequipement.station.mac == dest_mac) {
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