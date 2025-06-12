#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Conversion d'une chaîne MAC en structure AdresseMAC
AdresseMAC convertir_en_mac(const char* mac_str) {
    AdresseMAC mac;
    sscanf(mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &mac.octets[0], &mac.octets[1], &mac.octets[2],
           &mac.octets[3], &mac.octets[4], &mac.octets[5]);
    return mac;
}

// Conversion d'une chaîne IP en structure AdresseIP
AdresseIP convertir_en_ip(const char* ip_str) {
    AdresseIP ip;
    sscanf(ip_str, "%hhu.%hhu.%hhu.%hhu",
           &ip.octets[0], &ip.octets[1], &ip.octets[2], &ip.octets[3]);
    return ip;
}

// Fonctions de comparaison
int mac_egal(AdresseMAC m1, AdresseMAC m2) {
    return memcmp(m1.octets, m2.octets, MAC_ADDR_LEN) == 0;
}

int ip_egal(AdresseIP ip1, AdresseIP ip2) {
    return memcmp(ip1.octets, ip2.octets, IP_ADDR_LEN) == 0;
}