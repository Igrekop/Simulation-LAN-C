#ifndef RESEAU_H
#define RESEAU_H

#include <stdio.h>
#include <stdint.h>
//Chatos
typedef struct ReseauLocal ReseauLocal;

// Adresse MAC = 6 octets = 48 bits -> on utilise un entier 64 bits (on n'utilise que les 48 bits de poids faible)
typedef uint64_t AdresseMAC;

// Adresse IP = 4 octets = 32 bits
typedef uint32_t AdresseIP;

// Fonctions d'affichage
void afficher_mac(AdresseMAC mac);
void afficher_ip(AdresseIP ip);


// Etape 2
// Fonction pour lire le fichier obtenu (mylan) qui passe d'un string a mac ou ip
AdresseMAC convertir_en_mac(const char* mac_str);
AdresseIP convertir_en_ip(const char* ip_str);

//Fonction qui charge tout un reseau depuis mylan
int charger_reseau_fichier(const char* nom_fichier, ReseauLocal* reseau);

#endif