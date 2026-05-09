//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_PLAYER_H
#define PROJET_ALGO_S2_PLAYER_H

#include <allegro.h>
#include "../headers/assets.h"

#define MAX_LASERS 16

typedef struct Laser {
    float x, y;
    float dy;
    int active;
    int frame;
    int frame_timer;
} Laser;

typedef struct Player {
    float x, y;
    float dx, dy;
    float ddx, ddy;
    float vmax;
    int vies;
    int invincible;
    int invincible_timer;
    int skin_id;
    Laser *lasers;
    int laser_count;
    char pseudo[20];
} Player;

void player_init (Player *p);
void player_update (Player *p);
void player_move_left (Player *p);
void player_move_right (Player *p);
void player_draw (BITMAP *buffer, Player *p, Assets *a);
void player_destroy (Player **p);
void player_shot (Player *p);

#endif //PROJET_ALGO_S2_PLAYER_H