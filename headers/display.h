//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_DISPLAY_H
#define PROJET_ALGO_S2_DISPLAY_H

#include <allegro.h>

#include "assets.h"
#include "player.h"

void initialisation_allegro();  // initialisation de allegro
void init_display(BITMAP **buffer, Assets *assets);   // initialisation du buffer et des assets
BITMAP *player_sprite (Assets assets);
void display(BITMAP *buffer, Assets *assets, Player *player); // affichage
void destroy_display(BITMAP **buffer, Assets *assets); // liberation du buffer

#endif //PROJET_ALGO_S2_DISPLAY_H
