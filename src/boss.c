//
// Created by Ghiwane on 24/04/2026.
//

#include "../headers/boss.h"
#include <stdlib.h>

void boss_move_left(Boss *b) {
    if (b->dx > -b->vmax) b->dx -= b->ddx;
}

void boss_move_right(Boss *b) {
    if (b->dx < b->vmax) b->dx += b->ddx;
}

// Choisit et initialise une nouvelle séquence d'éclair aléatoire
static void boss_start_new_seq(Boss *b) {
    b->current_seq = rand() % ECLAIR_SEQ_COUNT;
    b->seq_phase   = 0;
    b->seq_pulse_on = 0;

    switch (b->current_seq) {
        case ECLAIR_SEQ_LONG:
            // Tir long continu : 3 à 5 secondes
            b->eclair_timer  = 0;
            b->seq_timer     = 180 + rand() % 120;
            break;
        case ECLAIR_SEQ_RAFALE:
            // Enchainement de tirs courts pendant le déplacement
            b->seq_timer = 5 + rand() % 4;  // durée d'un pulse ON (en frames)
            b->eclair_timer = 0;
            break;
        case ECLAIR_SEQ_PULSE:
            // Pulses rapides : on/off très courts
            b->seq_timer = 3 + rand() % 3;
            b->eclair_timer = 0;
            break;
        default:
            b->eclair_timer = 180 + rand() % 300;
            break;
    }
}

// Met à jour la séquence d'éclair en cours
static void boss_update_eclair_seq(Boss *b) {
    switch (b->current_seq) {

        case ECLAIR_SEQ_LONG:
            // Un seul tir continu, puis cooldown
            if (b->seq_phase == 0) {
                b->eclair_active = 1;
                b->seq_timer--;
                if (b->seq_timer <= 0) {
                    b->eclair_active = 0;
                    b->seq_phase     = 1;
                    b->eclair_timer  = 200 + rand() % 200;
                }
            } else {
                // Cooldown avant prochaine séquence
                b->eclair_timer--;
                if (b->eclair_timer <= 0)
                    boss_start_new_seq(b);
            }
            break;

        case ECLAIR_SEQ_RAFALE: {
            // Tirs courts intercalés de pauses, boss en mouvement forcé
            b->moving   = 1;
            b->seq_timer--;
            if (b->seq_timer <= 0) {
                b->seq_pulse_on  = !b->seq_pulse_on;
                b->eclair_active = b->seq_pulse_on;
                if (b->seq_pulse_on)
                    b->seq_timer = 6 + rand() % 5;    // durée ON
                else
                    b->seq_timer = 10 + rand() % 10;   // durée OFF (pause)
                b->seq_phase++;
            }
            // Après 8 cycles (4 tirs), fin de séquence
            if (b->seq_phase >= 8) {
                b->eclair_active = 0;
                b->eclair_timer  = 150 + rand() % 150;
                boss_start_new_seq(b);
            }
            break;
        }

        case ECLAIR_SEQ_PULSE:
            // Pulses très rapides
            b->seq_timer--;
            if (b->seq_timer <= 0) {
                b->seq_pulse_on  = !b->seq_pulse_on;
                b->eclair_active = b->seq_pulse_on;
                b->seq_timer     = 3 + rand() % 3;
                b->seq_phase++;
            }
            // Fin après 12 cycles
            if (b->seq_phase >= 12) {
                b->eclair_active = 0;
                b->eclair_timer  = 180 + rand() % 200;
                boss_start_new_seq(b);
            }
            break;

        default:
            break;
    }
}

void boss_update(Boss *b) {
    if (!b->active) return;

    // Séquence de mort : tremblement et mini-explosions
    if (b->dying) {
        b->die_timer++;
        // Tremblement gauche/droite
        b->shake_x = (rand() % 11 - 5) * 1.5f;

        // Mini-explosions toutes les ~18 frames
        b->exp_mini_timer--;
        if (b->exp_mini_timer <= 0) {
            b->exp_mini_timer  = 14 + rand() % 10;
            b->exp_mini_active = 1;
            b->exp_mini_frame  = 0;
            // Position aléatoire autour du boss
            b->exp_mini_x = b->x + rand() % 280 + 10;
            b->exp_mini_y = b->y + rand() % 160 + 10;
        }

        // Avance la frame de mini-explosion
        if (b->exp_mini_active) {
            b->exp_mini_frame++;
            if (b->exp_mini_frame >= 50)
                b->exp_mini_active = 0;
        }

        // Après 3 secondes (180 frames), fin de la séquence de tremblement
        if (b->die_timer >= 180) {
            b->dying     = 0;
            b->death_done = 1;
            b->shake_x   = 0;
        }
        return;
    }

    // Déplacement normal
    if (b->moving) {
        if (b->direction == 1) boss_move_right(b);
        else boss_move_left(b);
    }

    // Timer de mouvement / repos
    b->move_timer--;
    if (b->move_timer <= 0) {
        if (b->moving) {
            b->moving     = 0;
            b->move_timer = 300;
        } else {
            b->moving     = 1;
            b->direction  = (rand() % 2 == 0) ? 1 : -1;
            b->move_timer = 60 + rand() % 60;
        }
    }

    // Position horizontale avec limites
    float new_x = b->x + b->dx;
    if (new_x < 0) {
        b->x      = 0;
        b->direction = 1;
    } else if (new_x + 300 > 1440) {   // SCREEN_W
        b->x      = 1440 - 300;
        b->direction = -1;
    } else {
        b->x = new_x;
    }
    b->dx *= 0.95f;

    // Mise à jour de la séquence d'éclair
    boss_update_eclair_seq(b);
}

void boss_init(Boss *b) {
    b->x        = 1440 / 2.0f - 150;
    b->y        = 80;   // légèrement plus bas pour laisser la place à la barre de PV
    b->pv       = BOSS_MAX_PV;
    b->pv_max   = BOSS_MAX_PV;
    b->vmax     = 15.0f;
    b->dx       = 0.0f;
    b->ddx      = 1.5f;
    b->eclairs  = malloc(BOSS_MAX_ECLAIRS * sizeof(Eclair));
    b->eclair_count  = BOSS_MAX_ECLAIRS;
    b->move_timer    = 60;
    b->moving        = 0;
    b->direction     = 1;
    b->exp_frame     = 0;
    b->exp_timer     = 0;
    b->active        = 1;
    b->eclair_timer  = 300;
    b->eclair_active = 0;

    // Séquences
    b->current_seq   = ECLAIR_SEQ_LONG;
    b->seq_timer     = 0;
    b->seq_phase     = 0;
    b->seq_pulse_on  = 0;

    // Cinématique de mort
    b->dying           = 0;
    b->die_timer       = 0;
    b->shake_x         = 0;
    b->exp_mini_timer  = 20;
    b->exp_mini_frame  = 0;
    b->exp_mini_active = 0;
    b->exp_mini_x      = 0;
    b->exp_mini_y      = 0;
    b->death_done      = 0;
}
