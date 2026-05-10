//
// Created by Ghiwane on 24/04/2026.
//

#ifndef PROJET_ALGO_S2_BOSS_H
#define PROJET_ALGO_S2_BOSS_H

#include "assets.h"

#define BOSS_MAX_PV      700
#define BOSS_MAX_ECLAIRS 8

// Séquences de tir d'éclair disponibles
typedef enum {
    ECLAIR_SEQ_LONG,        // Un long tir continu
    ECLAIR_SEQ_RAFALE,      // Enchainement de tirs courts en se déplaçant
    ECLAIR_SEQ_PULSE,       // Tirs en pulses rapides
    ECLAIR_SEQ_COUNT
} EclairSequence;

// Structure d'un éclair individuel
typedef struct Eclair {
    float x, y;
    float dy;
    int active;
} Eclair;

// Structure du boss
typedef struct Boss {
    float x, y;         // position
    float dx;           // vitesse
    float ddx;          // accélération
    float pv;
    float pv_max;
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

    // Séquences d'éclair diversifiées
    EclairSequence current_seq;
    int seq_timer;
    int seq_phase;      // phase interne de la séquence
    int seq_pulse_on;   // état on/off pour les pulses

    // Cinématique de victoire (tremblement + explosions)
    int dying;          // 1 = séquence de mort en cours
    int die_timer;      // timer global de la séquence de mort
    float shake_x;      // décalage de tremblement
    int exp_mini_timer; // timer entre les mini-explosions
    int exp_mini_frame; // frame de la mini-explosion courante
    int exp_mini_active;
    float exp_mini_x;   // position de la mini-explosion
    float exp_mini_y;
    int death_done;     // 1 quand la séquence de mort est terminée
} Boss;

void boss_start_new_seq(Boss *b);
void boss_update_eclair_seq(Boss *b);
void boss_init(Boss *b);
void boss_move_right(Boss *b);
void boss_move_left(Boss *b);
void boss_update(Boss *b);
void boss_shoot(Boss *b);
void boss_draw(BITMAP *buffer, Boss *b);
void boss_destroy(Boss *b);

#endif //PROJET_ALGO_S2_BOSS_H
