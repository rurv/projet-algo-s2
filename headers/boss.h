//
// Created by Ghiwane on 24/04/2026.
//

#ifndef PROJET_ALGO_S2_BOSS_H
#define PROJET_ALGO_S2_BOSS_H

#include "assets.h"

#define BOSS_MAX_PV     100
#define BOSS_MAX_ECLAIRS 8

// structure eclair du boss
typedef struct Eclair {
    float x, y;
    float dy;
    int active;
} Eclair;

// structure du boss
typedef struct Boss {
    float x, y; // position
    float dx;   // vitesse
    float ddx;  // acceleration
    float pv;
    int active;
    int warning;
    int warning_timer;
    int shoot_timer;
    float vmax;
    Eclair *eclairs;
    int eclair_count;
    int move_timer;     // durée du mouvement ou du repos
    int moving;         // 1 = en mouvement, 0 = au repos
    int direction;      // 1 = droite, -1 = gauche
    int exp_frame;
    int exp_timer;
    int eclair_timer;
    int eclair_active;
} Boss;

void boss_init(Boss *b);                    // initialisation du boss
void boss_move_right (Boss *b);             // mouvement vers la droite
void boss_move_left (Boss *b);              // mouvement vers la gauche
void boss_update(Boss *b);                  // mise à jour du boss
void boss_shoot(Boss *b);                   // tir du boss
void boss_draw(BITMAP *buffer, Boss *b);    // affichage du boss
void boss_destroy(Boss *b);                 // animation de destruction du boss

#endif //PROJET_ALGO_S2_BOSS_H