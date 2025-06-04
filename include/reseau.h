#ifndef RESEAU_H
#define RESEAU_H

#include <stdio.h>
#include <stdint.h>

// Adresse MAC = 6 octets = 48 bits -> on utilise un entier 64 bits (on n'utilise que les 48 bits de poids faible)
typedef uint64_t AdresseMAC;

// Adresse IP = 4 octets = 32 bits
typedef uint32_t AdresseIP;

// Fonctions d'affichage
void afficher_mac(AdresseMAC mac);
void afficher_ip(AdresseIP ip);

#endif