//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_PLAYER_H
#define PROJET_ALGO_S2_PLAYER_H

#include <allegro.h>
#include "../headers/assets.h"

typedef struct Player {
    int x, y;
    int skin_id;
} Player;

Player init_player (void);
void draw_player (BITMAP **buffer, Assets assets, Player p);

#endif //PROJET_ALGO_S2_PLAYER_H