#include <stdio.h>
#include "ip_adresse.h"

void printIPv4Address(AdresseIPv4 ip) {
    printf("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}
