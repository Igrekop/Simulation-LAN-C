#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#define MAC_ADDR_LEN 6
#define IP_ADDR_LEN 4

// Types d'adresses
typedef struct {
    uint8_t addr[MAC_ADDR_LEN];
} mac_addr_t;

typedef struct {
    uint8_t addr[IP_ADDR_LEN];
} ip_addr_t;

#endif 