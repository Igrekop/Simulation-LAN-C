#include <stdio.h>
#include <mac_adresse.h>

void AfficherMAC(AdresseMAC mac) {
    printf("%02X:%02X:%02X:%02X:%02X:%02X\n",mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}
