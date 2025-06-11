    #include <stdio.h>
    #include <stdint.h>
    #include <string.h>   // Pour strlen, sscanf
    #include <stdlib.h>   // Pour exit, ou strtol si besoin
    #include "reseau.h"
    #include "equipement.h"

    void afficher_mac(AdresseMAC mac) {
        for (int i = 5; i >= 0; i--) {
            // décalage et masquage ex si i = 5 mac > 40 donc si mac = 00XXXXXX alors mac = 00
            unsigned int octet = (mac >> (8 * i)) & 0xFF; // 0xFF = 11111111
            printf("%02x", octet);
            if (i > 0) printf(":");
        }
    }

    // même principe
    void afficher_ip(AdresseIP ip) {
        for (int i = 3; i >= 0; i--) {
            printf("%d", (ip >> (8 * i)) & 0xFF);
            if (i > 0) printf(".");
        }
    }

    //Etape 2 

    // fonction pour convertir une chaine MAC (xx:xx..) en AdresseMAC (uint64_t)
    AdresseMAC convertir_en_mac(const char* mac_str) {
        AdresseMAC mac = 0;
        unsigned int octets[6];
        if (sscanf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
                &octets[0], &octets[1], &octets[2],
                &octets[3], &octets[4], &octets[5]) != 6) {
            fprintf(stderr, "Erreur de format MAC: %s\n", mac_str);
            return 0; 
        }

        // Combine les octets en un seul uint64_t
        for (int i = 0; i < 6; i++) {
            mac = (mac << 8) | octets[i];
        }
        return mac;
    }

    // Fonction pour convertir une chaîne IP (xx:xx:xx:xx) en AdresseIP (uint32_t)
    AdresseIP convertir_en_ip(const char* ip_str) {
        AdresseIP ip = 0;
        unsigned int octets[4];
        if (sscanf(ip_str, "%d.%d.%d.%d",
                &octets[0], &octets[1], &octets[2], &octets[3]) != 4) {
            fprintf(stderr, "Erreur de format IP: %s\n", ip_str);
            return 0; // Retourne 0 en cas d'erreur de parsing
        }

        // Combine les octets en un seul uint32_t
        ip = (octets[0] << 24) | (octets[1] << 16) | (octets[2] << 8) | octets[3];
        return ip;
    }

    // Fonction principale pour charger le réseau depuis un fichier de configuration
    int charger_reseau_fichier(const char* nom_fichier, ReseauLocal* reseau) {
        FILE* fichier = fopen(nom_fichier, "r");
        if (fichier == NULL) {
            perror("Erreur lors de l'ouverture du fichier de configuration");
            return -1; 
        }

        int nb_equipements_lus;
        int nb_liens_lus;
        // Lecture de la première ligne d'en-tête
        if (fscanf(fichier, "%d %d\n", &nb_equipements_lus, &nb_liens_lus) != 2) {
            fprintf(stderr, "Erreur de lecture de l'en-tête du fichier.\n");
            fclose(fichier);
            return -1;
        }

        reseau->nb_equipements = nb_equipements_lus;

        char line_buffer[256]; // pour stocker des lignes entières (max 256)

        // Lecture des équipements
        for (int i = 0; i < reseau->nb_equipements; i++) {
            if (fgets(line_buffer, sizeof(line_buffer), fichier) == NULL) {
                fprintf(stderr, "Erreur de lecture des équipements ou fin de fichier inattendue.\n");
                fclose(fichier);
                return -1;
            }
            // Supprime le caractère de nouvelle ligne '\n' si présent
            line_buffer[strcspn(line_buffer, "\n")] = '\0';

            int type_equipement_lue;
            char mac_str[20];
            char ip_str[20]; // Pour les stations
            int nb_ports_lue, priorite_lue; // Pour les switchs

            // Lisez le type d'équipement
            if (sscanf(line_buffer, "%d;", &type_equipement_lue) != 1) {
                fprintf(stderr, "Erreur de parsing du type d'équipement sur la ligne: %s\n", line_buffer);
                fclose(fichier);
                return -1;
            }
            reseau->equipements[i].type = (TypeEquipement)type_equipement_lue;

            // Utilisez une chaîne temporaire pour le reste de la ligne après le type
            char *ptr_after_type = strchr(line_buffer, ';');
            if (ptr_after_type == NULL) {
                fprintf(stderr, "Erreur: ';' manquant après le type sur la ligne: %s\n", line_buffer);
                fclose(fichier);
                return -1;
            }
            ptr_after_type++; // Avance après le ';'

            if (reseau->equipements[i].type == SWITCH) {
                // Pour un switch: "MAC;NB_PORTS;PRIORITE"
                if (sscanf(ptr_after_type, "%17[^;];%d;%d", mac_str, &nb_ports_lue, &priorite_lue) != 3) {
                    fprintf(stderr, "Erreur de parsing des données du switch sur la ligne: %s\n", line_buffer);
                    fclose(fichier);
                    return -1;
                }
                reseau->equipements[i].typequipement.sw.mac = convertir_en_mac(mac_str);
                reseau->equipements[i].typequipement.sw.nb_ports = nb_ports_lue;
                reseau->equipements[i].typequipement.sw.priorite = priorite_lue;
                // Initialisation de la table de commutation
                for(int j=0; j<MAX_PORTS; j++) {
                    reseau->equipements[i].typequipement.sw.table_commutation[j] = 0;
                }

            } else if (reseau->equipements[i].type == STATION) {
                // Pour une station: "MAC;IP"
                if (sscanf(ptr_after_type, "%17[^;];%15s", mac_str, ip_str) != 2) {
                    fprintf(stderr, "Erreur de parsing des données de la station sur la ligne: %s\n", line_buffer);
                    fclose(fichier);
                    return -1;
                }
                reseau->equipements[i].typequipement.station.mac = convertir_en_mac(mac_str);
                reseau->equipements[i].typequipement.station.ip = convertir_en_ip(ip_str);
            } else {
                fprintf(stderr, "Type d'équipement inconnu sur la ligne: %s\n", line_buffer);
                fclose(fichier);
                return -1;
            }
        }

        // Lecture des liens
        for (int k = 0; k < nb_liens_lus; k++) {
            int eq1, eq2, poids;
            if (fscanf(fichier, "%d;%d;%d\n", &eq1, &eq2, &poids) != 3) {
                fprintf(stderr, "Erreur de lecture des liens ou fin de fichier inattendue.\n");
                fclose(fichier);
                return -1;
            }
            if (eq1 >= 0 && eq1 < reseau->nb_equipements &&
                eq2 >= 0 && eq2 < reseau->nb_equipements) {
                reseau->matrice_adjacence[eq1][eq2] = poids;
                reseau->matrice_adjacence[eq2][eq1] = poids; // Le graphe est non orienté (liens bidirectionnels)
            } else {
                fprintf(stderr, "Indice d'équipement invalide pour le lien : %d;%d;%d\n", eq1, eq2, poids);
            }
        }

        fclose(fichier); // Fermeture du fichier
        return 0; // Retourne 0 pour succès
    }
