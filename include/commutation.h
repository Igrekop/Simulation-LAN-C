#ifndef COMMUTATION_H
#define COMMUTATION_H

#include "equipement.h"
#include "trame.h"

int switch_apprendre_mac(switch_t *sw, mac_addr_t mac_src, int port);
int switch_rechercher_port(switch_t *sw, mac_addr_t mac_dest);
void afficher_table_mac(switch_t *sw);

#endif 