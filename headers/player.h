//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_PLAYER_H
#define PROJET_ALGO_S2_PLAYER_H

#include <allegro.h>
#include "../headers/assets.h"

typedef struct Player {
    float x, y;
    float dx, dy;
    int pv;
    int invincible;
    int skin_id;
} Player;

void player_init (Player **p);
void player_update (Player *p);
void player_draw (BITMAP *buffer, Player *p, Assets *a);
void player_destroy (Player **p);

#endif //PROJET_ALGO_S2_PLAYER_H