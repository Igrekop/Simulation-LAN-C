#ifndef RESEAU_H
#define RESEAU_H

#include "equipement.h"
#include "affichage.h"
#include "conversion.h"

// Fonctions de gestion du réseau
void init_reseau(reseau_t* reseau);
int ajouter_station(reseau_t* reseau, mac_addr_t mac, ip_addr_t ip);
int ajouter_switch(reseau_t* reseau, mac_addr_t mac, int nb_ports, int priority);
int ajouter_lien(reseau_t* reseau, int equip1, int equip2, int poids);
int charger_reseau_fichier(const char* nom_fichier, reseau_t* reseau);
void afficher_table_commutation(switch_t* sw);

#endif 