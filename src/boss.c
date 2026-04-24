//
// Created by Ghiwane on 24/04/2026.
//

#include "../headers/boss.h"

void boss_move_left (Boss *b) {
    if (b->dx > -b->vmax) b->dx -= b->ddx;
}

void boss_move_right (Boss *b) {
    if (b->dx < b->vmax) b->dx += b->ddx;
}

void Boss_update (Boss *b) {
    float new_x = b->x + b->dx;
    if (new_x < 0) b->x = 0;
    else if (new_x + 60 > SCREEN_W) b->x = SCREEN_W - 60;
    else b->x = new_x;
    b->dx *= 0.95;
}