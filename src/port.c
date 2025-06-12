#include "../include/port.h"
#include <string.h>

void initialiser_port(Port *port, int id) {
    port->id = id;
    port->status = PORT_DISCONNECTED;
    port->connected_to_equip = -1;
    port->connected_to_port = -1;
    memset(&port->last_src_mac, 0, sizeof(AdresseMAC));
}

void connecter_ports(Port *port1, int equip1, Port *port2, int equip2) {
    port1->status = PORT_CONNECTED;
    port1->connected_to_equip = equip2;
    port1->connected_to_port = port2->id;

    port2->status = PORT_CONNECTED;
    port2->connected_to_equip = equip1;
    port2->connected_to_port = port1->id;
}

void deconnecter_port(Port *port) {
    port->status = PORT_DISCONNECTED;
    port->connected_to_equip = -1;
    port->connected_to_port = -1;
}

int trouver_port_par_mac(const PortManager *pm, const AdresseMAC *mac) {
    for (int i = 0; i < pm->nb_ports; i++) {
        if (pm->ports[i].status == PORT_CONNECTED &&
            memcmp(&pm->ports[i].last_src_mac, mac, sizeof(AdresseMAC)) == 0) {
            return i;
        }
    }
    return -1;
}

void mettre_a_jour_table_commutation(Switch *sw, const AdresseMAC *mac, int port_id) {
    (void)port_id; // évite le warning de paramètre inutilisé
    // Vérifier si l'adresse MAC existe déjà dans la table
    for (int i = 0; i < MAX_PORTS; i++) {
        if (memcmp(&sw->table_commutation[i], mac, sizeof(AdresseMAC)) == 0) {
            // L'adresse existe déjà, on met à jour le port
            sw->table_commutation[i] = *mac;
            return;
        }
    }

    // Si l'adresse n'existe pas, on cherche un emplacement vide
    for (int i = 0; i < MAX_PORTS; i++) {
        AdresseMAC mac_vide = {0};
        if (memcmp(&sw->table_commutation[i], &mac_vide, sizeof(AdresseMAC)) == 0) {
            sw->table_commutation[i] = *mac;
            return;
        }
    }
}