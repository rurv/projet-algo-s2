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

void boss_update(Boss *b) {
    // déplacement
    if (b->active==0) b->moving = 0;
    if (b->moving) {
        if (b->direction == 1) boss_move_right(b);
        else boss_move_left(b);
    }

    // timer
    b->move_timer--;

    if (b->move_timer <= 0) {
        if (b->moving) {
            // passe en repos
            b->moving = 0;
            b->move_timer = 300;
        } else {
            // repart dans une direction aléatoire
            b->moving = 1;
            b->direction = (rand() % 2 == 0) ? 1 : -1;
            b->move_timer = 60 +rand() % 60;  // bouge 1 à 2 sec
        }
    }

    // position
    float new_x = b->x + b->dx;
    if (new_x < 0) { b->x = 0; b->direction = 1; }
    else if (new_x + 300 > SCREEN_W) { b->x = SCREEN_W - 300; b->direction = -1; }
    else b->x = new_x;

    b->dx *= 0.95;

    b->eclair_timer--;

    if (b->eclair_timer <= 0 && b->eclair_timer > -100) {
        b->eclair_active = 1;      // actif pendant 100 frames
    } else {
        b->eclair_active = 0;
        if (b->eclair_timer <= -100) {
            b->eclair_timer = 100 + rand()%301 ; // réinitialise le cooldown
        }
    }
}

void boss_init(Boss *b) {
    b->x = SCREEN_W/2.0 - 150 ; b->y = 25;
    b->pv = BOSS_MAX_PV;
    b->vmax = 15.0;
    b->dx = 0.0;
    b->ddx = 1.5;
    b->eclairs = malloc(BOSS_MAX_ECLAIRS * sizeof(Eclair));
    b->eclair_count = BOSS_MAX_ECLAIRS;
    b->move_timer = 60;
    b->moving = 0;
    b->direction = 1;
    b->exp_frame = 0;
    b->exp_timer = 0;
    b->active=1;
    b->eclair_timer=300;
    b->eclair_active=0;
}
