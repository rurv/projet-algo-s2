//
// Created by ruben on 4/2/2026.
//

#include "../headers/player.h"

void player_init (Player *p) {
    p->x = SCREEN_W/2.0; p->y = SCREEN_H/5.0*4.0;
    p->dx = 0.0; p->dy = 0.0;
    p->ddx = 0.5; p->ddy = 0.0;
    p->vmax = 5.0;
    p->pv = 10; p->invincible = 0;
    p->skin_id = 0;
}

void player_update (Player *p) {
    float new_x = p->x + p->dx;
    if (new_x > 0 && new_x < SCREEN_W) p->x = new_x;
    p->dx *= 0.95;
}

void player_move_left (Player *p) {
    if (p->dx > -p->vmax) p->dx -= p->ddx;
}

void player_move_right (Player *p) {
    if (p->dx < p->vmax) p->dx += p->ddx;
}

