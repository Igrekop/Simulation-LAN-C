#include "equipement.h"
#include <stdio.h>
#include <string.h>

// Convertit une chaîne en adresse MAC
mac_addr_t convertir_en_mac(const char* mac_str) {
    mac_addr_t mac;
    unsigned int bytes[MAC_ADDR_LEN];
    sscanf(mac_str, "%x:%x:%x:%x:%x:%x",
           &bytes[0], &bytes[1], &bytes[2],
           &bytes[3], &bytes[4], &bytes[5]);
    for (int i = 0; i < MAC_ADDR_LEN; i++) {
        mac.addr[i] = (uint8_t)bytes[i];
    }
    return mac;
}

// Convertit une chaîne en adresse IP
ip_addr_t convertir_en_ip(const char* ip_str) {
    ip_addr_t ip;
    unsigned int bytes[IP_ADDR_LEN];
    sscanf(ip_str, "%u.%u.%u.%u",
           &bytes[0], &bytes[1], &bytes[2], &bytes[3]);
    for (int i = 0; i < IP_ADDR_LEN; i++) {
        ip.addr[i] = (uint8_t)bytes[i];
    }
    return ip;
}

// Compare deux adresses MAC
int mac_egal(mac_addr_t m1, mac_addr_t m2) {
    for (int i = 0; i < MAC_ADDR_LEN; i++) {
        if (m1.addr[i] != m2.addr[i]) return 0;
    }
    return 1;
}

// Compare deux adresses IP
int ip_egal(ip_addr_t ip1, ip_addr_t ip2) {
    for (int i = 0; i < IP_ADDR_LEN; i++) {
        if (ip1.addr[i] != ip2.addr[i]) return 0;
    }
    return 1;
}