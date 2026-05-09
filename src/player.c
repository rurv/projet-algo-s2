//
// Created by ruben on 4/2/2026.
//

#include "../headers/player.h"

void player_init (Player *p) {
    p->x = SCREEN_W/2.0; p->y = SCREEN_H/5.0*4.0;
    p->dx = 0.0; p->dy = 0.0;
    p->ddx = 1.5; p->ddy = 0.0;
    p->vmax = 15.0;
    p->vies = 3; p->invincible = 0;
    p->invincible_timer=0;
    p->skin_id = 0;
    p->lasers = malloc(MAX_LASERS * sizeof(Laser));
    p->laser_count = MAX_LASERS;

    for (int i = 0; i < MAX_LASERS; i++) {
        p->lasers[i].active = 0;
        p->lasers[i].frame = 0;
        p->lasers[i].frame_timer = 0;
    }
}

void player_shot(Player *p) {
    for (int i = 0; i < p->laser_count; i++) {
        if (!p->lasers[i].active) {
            p->lasers[i].x = p->x + 24 ; // centré sur le vaisseau
            p->lasers[i].y = p->y;
            p->lasers[i].dy = -30.0;
            p->lasers[i].active = 1;
            return; // on tire un seul laser à la fois
        }
    }
}

void player_update (Player *p) {
    float new_x = p->x + p->dx;
    if (new_x < 0) p->x = 0;
    else if (new_x + 60 > SCREEN_W) p->x = SCREEN_W - 60;
    else p->x = new_x;
    p->dx *= 0.90;

    for (int i = 0; i < p->laser_count; i++) {
        if (p->lasers[i].active) {
            p->lasers[i].y += p->lasers[i].dy;
            p->lasers[i].frame_timer++;
            if (p->lasers[i].frame_timer >= 5) {
                p->lasers[i].frame_timer = 0;
                p->lasers[i].frame = (p->lasers[i].frame + 1) % 4;
            }
            if (p->lasers[i].y < 0)
                p->lasers[i].active = 0;
        }
    }
}

void player_move_left (Player *p) {
    if (p->dx > -p->vmax) p->dx -= p->ddx;
}

void player_move_right (Player *p) {
    if (p->dx < p->vmax) p->dx += p->ddx;
}

void player_destroy(Player **p) {
    free((*p)->lasers);
    free(*p);
    *p = NULL;
}


