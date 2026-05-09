//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_MENU_H
#define PROJET_ALGO_S2_MENU_H

#include "config.h"
#include "assets.h"
#include <allegro.h>
#include "display.h"
#include "player.h"

enum EcranActuel { MENU_PRINCIPAL, SAISIE_PSEUDO, REGLE, DECOMPTE, CHOIX, JEU, REPRENDRE, QUITTER, GAME_OVER };
enum DiferrentDecompte {DECOMPTE1, DECOMPTE2, DECOMPTE3, DECOMPTEY, DECOMPTEN};

// Macros pratiques pour convertir des pourcentages en pixels
// Ex: LX(0.5) = 50% de la largeur = 400px sur un écran 800px
#define LX(p) ((int)(SCREEN_WIDTH * (p)))
#define LY(p) ((int)(SCREEN_HEIGHT * (p)))

#define VAISSEAU_L 0.03
#define VAISSEAU_H 0.078

#define CLAUDE_L 0.14
#define CLAUDE_H 0.17

#define NOMBRE_ETOILES 150

#define ATTENDRE_RELACHE() do { while (mouse_b & 1) rest(10); } while(0)

typedef struct {
    int x, y;
    int vitesse;
    int luminosite;
} Etoile;

typedef struct {
    int hauteur, longueure;
    BITMAP *perso;
} Personnage;

typedef struct {
    int x, y;
    int vitesse;
    BITMAP *skin;
} Vaisseau;

void ecrire_centre_texte(BITMAP *dest, const char *texte, int centre_x, int y, int couleur, float multiplicateur);
void ecrire_texte(BITMAP *dest, const char *texte, int x, int y, int couleur, float multiplicateur);
enum EcranActuel menu_principale(enum EcranActuel ecran, BITMAP *buffer );
enum EcranActuel saisie_pseudo(enum EcranActuel ecran, BITMAP *buffer, Player *p );
enum EcranActuel regles(enum EcranActuel ecran, BITMAP *buffer);
enum EcranActuel choix(enum EcranActuel ecran, BITMAP *buffer,Assets *assets, Player *p);
enum EcranActuel decompte(enum EcranActuel ecran, BITMAP *buffer, Assets *assets, Player *p);
enum EcranActuel game_over_screen(enum EcranActuel ecran, BITMAP *buffer, Player *p);



#endif //PROJET_ALGO_S2_MENU_H