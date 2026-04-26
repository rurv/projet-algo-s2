//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_DISPLAY_H
#define PROJET_ALGO_S2_DISPLAY_H

#include <allegro.h>

#include "config.h"
#include "assets.h"
#include "player.h"
#include "boss.h"

typedef struct Bitmaps {
    BITMAP *buffer;
    BITMAP *ship;
    BITMAP *fond;
    BITMAP *asteroid;
    float fond_scroll_x;
} Bitmaps;

void initialisation_allegro();  // initialisation de allegro
void init_display(Bitmaps *b, Assets *assets);   // initialisation du buffer et des assets
BITMAP *player_sprite (Assets assets);
void display(Bitmaps *b, Assets *assets, Player *p, Boss *boss); // affichage
void display_eclair(Bitmaps *b, Boss *boss, int active, int boss_active); // affiche les eclairs du boss
void destroy_display(Bitmaps *b, Assets *assets); // liberation du buffer

#endif //PROJET_ALGO_S2_DISPLAY_H
