//
// Created by Ghiwane on 24/04/2026.
//

#ifndef PROJET_ALGO_S2_BOSS_H
#define PROJET_ALGO_S2_BOSS_H
#include "assets.h"
#define BOSS_MAX_PV     100
#define BOSS_MAX_ECLAIRS 8

typedef struct Eclair {
    float x, y;
    float dy;
    int active;
} Eclair;

typedef struct Boss {
    float x;
    float dx;
    float ddx;
    float pv;
    int active;
    int warning;
    int warning_timer;
    int shoot_timer;
    float vmax;
    Eclair *eclairs;
    int eclair_count;
} Boss;

void boss_init(Boss *b);
void boss_move_right (Boss *b);
void boss_move_left (Boss *b);
void boss_update(Boss *b);
void boss_shoot(Boss *b);
void boss_draw(BITMAP *buffer, Boss *b);
void boss_destroy(Boss *b);

#endif //PROJET_ALGO_S2_BOSS_H