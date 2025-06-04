#include <stdio.h>
#include "reseau.h"

void afficher_mac(AdresseMAC mac) {
    for (int i = 5; i >= 0; i--) {
        unsigned int octet = (mac >> (8 * i)) & 0xFF;
        printf("%02x", octet);
        if (i > 0) printf(":");
    }
}

void afficher_ip(AdresseIP ip) {
    for (int i = 3; i >= 0; i--) {
        printf("%d", (ip >> (8 * i)) & 0xFF);
        if (i > 0) printf(".");
    }
}
