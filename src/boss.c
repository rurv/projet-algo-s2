#include "../headers/boss.h"
#include <stdlib.h>

void boss_move_left(Boss *b) {
    if (b->dx > -b->vmax) b->dx -= b->ddx;
}

void boss_move_right(Boss *b) {
    if (b->dx < b->vmax) b->dx += b->ddx;
}

void boss_start_new_seq(Boss *b) {
    b->current_seq  = rand() % ECLAIR_SEQ_COUNT;
    b->seq_phase    = 0;
    b->seq_pulse_on = 0;
    switch (b->current_seq) {
        case ECLAIR_SEQ_LONG:
            b->eclair_timer = 0;
            b->seq_timer    = 180 + rand() % 120;
            break;
        case ECLAIR_SEQ_RAFALE:
            b->seq_timer    = 5 + rand() % 4;
            b->eclair_timer = 0;
            break;
        case ECLAIR_SEQ_PULSE:
            b->seq_timer    = 3 + rand() % 3;
            b->eclair_timer = 0;
            break;
        default:
            b->eclair_timer = 180 + rand() % 300;
            break;
    }
}

void boss_update_eclair_seq(Boss *b) {
    switch (b->current_seq) {
        case ECLAIR_SEQ_LONG:
            if (b->seq_phase == 0) {
                b->eclair_active = 1;
                b->seq_timer--;
                if (b->seq_timer <= 0) {
                    b->eclair_active = 0;
                    b->seq_phase     = 1;
                    b->eclair_timer  = 200 + rand() % 200;
                }
            } else {
                b->eclair_timer--;
                if (b->eclair_timer <= 0) boss_start_new_seq(b);
            }
            break;
        case ECLAIR_SEQ_RAFALE: {
            b->moving = 1;
            b->seq_timer--;
            if (b->seq_timer <= 0) {
                b->seq_pulse_on  = !b->seq_pulse_on;
                b->eclair_active = b->seq_pulse_on;
                if (b->seq_pulse_on) b->seq_timer = 6 + rand() % 5;
                else b->seq_timer = 10 + rand() % 10;
                b->seq_phase++;
            }
            if (b->seq_phase >= 8) {
                b->eclair_active = 0;
                b->eclair_timer  = 150 + rand() % 150;
                boss_start_new_seq(b);
            }
            break;
        }
        case ECLAIR_SEQ_PULSE:
            b->seq_timer--;
            if (b->seq_timer <= 0) {
                b->seq_pulse_on  = !b->seq_pulse_on;
                b->eclair_active = b->seq_pulse_on;
                b->seq_timer     = 3 + rand() % 3;
                b->seq_phase++;
            }
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
    if (b->dying) {
        b->die_timer++;
        b->shake_x = (rand() % 11 - 5) * 1.5f;
        b->exp_mini_timer--;
        if (b->exp_mini_timer <= 0) {
            b->exp_mini_timer  = 14 + rand() % 10;
            b->exp_mini_active = 1;
            b->exp_mini_frame  = 0;
            b->exp_mini_x = b->x + rand() % 280 + 10;
            b->exp_mini_y = b->y + rand() % 160 + 10;
        }
        if (b->exp_mini_active) {
            b->exp_mini_frame++;
            if (b->exp_mini_frame >= 50) b->exp_mini_active = 0;
        }
        if (b->die_timer >= 180) {
            b->dying      = 0;
            b->death_done = 1;
            b->shake_x    = 0;
        }
        return;
    }
    if (b->moving) {
        if (b->direction == 1) boss_move_right(b);
        else boss_move_left(b);
    }
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
    float new_x = b->x + b->dx;
    if (new_x < 0) {
        b->x         = 0;
        b->direction = 1;
    } else if (new_x + 300 > 1440) {
        b->x         = 1440 - 300;
        b->direction = -1;
    } else b->x = new_x;
    b->dx *= 0.95f;
    boss_update_eclair_seq(b);
}

void boss_init(Boss *b) {
    b->x        = 1440 / 2.0f - 150;
    b->y        = 80;
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
    b->current_seq   = ECLAIR_SEQ_LONG;
    b->seq_timer     = 0;
    b->seq_phase     = 0;
    b->seq_pulse_on  = 0;
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
