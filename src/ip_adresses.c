#include "ip_adresses.h"

void printIPv4Address(IPv4Address ip) {
    printf("%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}
