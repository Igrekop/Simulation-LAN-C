#ifndef CONVERSION_H
#define CONVERSION_H

#include "equipement.h"

// Fonctions de comparaison
int mac_egal(mac_addr_t m1, mac_addr_t m2);
int ip_egal(ip_addr_t ip1, ip_addr_t ip2);

// Fonctions de conversion
mac_addr_t convertir_en_mac(const char* mac_str);
ip_addr_t convertir_en_ip(const char* ip_str);

#endif 