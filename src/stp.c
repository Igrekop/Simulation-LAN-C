#include "stp.h"
#include <stdio.h>

// Structure simple pour retenir pour chaque switch : distance au root et port root
typedef struct {
    int distance;
    int port_root; // voisin par lequel passer pour joindre le root
} stp_info_t;

// Trouve l'index du root : plus petite priorité, puis plus petite MAC
int stp_trouver_root(reseau_t *reseau) {
    int idx_root = -1;
    int i, j;
    for (i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == SWITCH) {
            if (idx_root == -1) {
                idx_root = i;
            } else {
                switch_t *sw = &reseau->equipements[i].data.sw;
                switch_t *sw_root = &reseau->equipements[idx_root].data.sw;
                if (sw->priority < sw_root->priority) {
                    idx_root = i;
                } else if (sw->priority == sw_root->priority) {
                    for (j = 0; j < MAC_ADDR_LEN; j++) {
                        if (sw->mac.addr[j] < sw_root->mac.addr[j]) {
                            idx_root = i;
                            break;
                        } else if (sw->mac.addr[j] > sw_root->mac.addr[j]) {
                            break;
                        }
                    }
                }
            }
        }
    }
    return idx_root;
}

// BFS pour calculer la distance au root et mémoriser le port root pour chaque switch
void stp_calculer_spanning_tree(reseau_t *reseau) {
    // Pour l'instant, on active tous les ports
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == SWITCH) {
            switch_t *sw = &reseau->equipements[i].data.sw;
            for (int j = 0; j < sw->nb_ports; j++) {
                sw->port_etat[j] = 1; // Port actif
            }
        }
    }
}

// Affiche l'état des ports pour chaque switch
void stp_afficher_etat_ports(reseau_t *reseau) {
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == SWITCH) {
            switch_t *sw = &reseau->equipements[i].data.sw;
            printf("Switch %d :\n", i);
            for (int j = 0; j < sw->nb_ports; j++) {
                printf("  Port %d : %s\n", j, sw->port_etat[j] ? "ACTIF" : "BLOQUÉ");
            }
        }
    }
} 