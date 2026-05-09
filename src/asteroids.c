#include "../headers/asteroids.h"
#include "../headers/config.h"
#include <stdlib.h>
#include <math.h>
#include "../headers/game.h"

static int ground_y(void) {
    return SCREEN_HEIGHT / 5 * 4 + 84;
}

/* Dessine le sprite 128x128 centré sur (cx, cy), mis à l'échelle selon le rayon */
static void draw_asteroid(BITMAP *sprite, BITMAP *buf, float cx, float cy, float r, int frame) {
    int size = (int)(r * 2);
    if (size < 2) size = 2;

    if (sprite) {
        /* Le sprite existe : on l'étire, fond magenta masqué */
        masked_stretch_blit(sprite, buf,
                        frame*128, 0, 128, 128,
                        (int)(cx - r), (int)(cy - r), size, size);
    } else {
        /* Fallback cercle marron si sprite absent */
        circlefill(buf, (int)cx, (int)cy, (int)r, makecol(139, 90, 43));
        circle(buf,     (int)cx, (int)cy, (int)r, makecol(80, 50, 20));
    }
}

static void spawn_one(AsteroidManager *am) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (am->asteroids[i].active) continue;
        Asteroid *a = &am->asteroids[i];

        /* Rayon entre 0.5*max_size et 2*max_size */
        float min_r = am->max_size * 0.5f;
        float max_r = am->max_size * 2.0f;
        a->radius = min_r + (rand() % 1000) / 1000.0f * (max_r - min_r);
        if (a->radius < 4.0f) a->radius = 4.0f;

        int margin = (int)a->radius + 1;
        int spawn_w = SCREEN_WIDTH  - 2 * margin;
        int spawn_h = ground_y()    / 3;
        if (spawn_w < 1) spawn_w = 1;
        if (spawn_h < 1) spawn_h = 1;

        a->x  = margin + rand() % spawn_w;
        a->y  = margin + rand() % spawn_h;
        a->dx = (float)(rand() % 5 + 1) * (rand() % 2 ? 1.0f : -1.0f);
        a->dy = (float)(rand() % 3 + 1);
        a->active = 1;
        a->frame = rand() % 64;
        a->frame_timer = 0;
        am->spawned++;
        am->count++;
        return;
    }
}

void asteroids_init(AsteroidManager *am, float gravity, int to_spawn, float max_size) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) am->asteroids[i].active = 0;
    am->count       = 0;
    am->spawned     = 0;
    am->to_spawn    = to_spawn;
    am->spawn_timer = 0;
    am->gravity     = gravity;
    am->max_size    = max_size;
}

void asteroids_update(AsteroidManager *am) {
    /* Spawn un astéroïde toutes les 120 frames */
    if (am->spawned < am->to_spawn) {
        am->spawn_timer++;
        if (am->spawn_timer >= 120) {
            am->spawn_timer = 0;
            spawn_one(am);
        }
    }

    float g  = am->gravity / (60.0f * 60.0f);
    int   gy = ground_y();

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        Asteroid *a = &am->asteroids[i];
        if (!a->active) continue;

        a->dy += g;
        a->x  += a->dx;
        a->y  += a->dy;

        a->frame_timer++;
        if (a->frame_timer >= 4) {
            a->frame_timer = 0;
            a->frame = (a->frame + 1) % 64;
        }

        /* Rebonds */
        if (a->y + a->radius >= gy)                { a->y = gy - a->radius;           a->dy = -a->dy; }
        if (a->x - a->radius <= 0)                 { a->x = a->radius;                a->dx = -a->dx; }
        else if (a->x + a->radius >= SCREEN_WIDTH) { a->x = SCREEN_WIDTH - a->radius; a->dx = -a->dx; }
        if (a->y - a->radius <= 0)                 { a->y = a->radius;                a->dy = -a->dy; }
    }
}

void asteroids_draw(BITMAP *sprite, BITMAP *buffer, AsteroidManager *am) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        Asteroid *a = &am->asteroids[i];
        if (!a->active) continue;
        draw_asteroid(sprite, buffer, a->x, a->y, a->radius, a->frame);
    }
}

void asteroid_split(AsteroidManager *am, int idx, Game *g) {
    Asteroid *a = &am->asteroids[idx];
    if (!a->active) return;

    float new_r = a->radius / 2.0f;
    // Sauvegarder avant de désactiver
    float save_x = a->x, save_y = a->y;
    float save_dx = a->dx, save_dy = a->dy;

    a->active = 0;
    am->count--;

    if (new_r < MIN_ASTEROID_SIZE) return;

    for (int frag = 0; frag < 2; frag++) {
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (am->asteroids[i].active) continue;
            Asteroid *b = &am->asteroids[i];
            b->radius = new_r;
            b->x  = save_x + (frag == 0 ? -new_r : new_r);
            b->y  = save_y;
            b->dx = save_dx * (frag == 0 ? -1.2f : 1.2f);
            b->dy = save_dy - 1.0f;
            b->active = 1;
            b->frame = rand() % 64;
            b->frame_timer = 0;
            am->count++;
            break;
        }
    }
    if (!g->bonus.actif && (rand() % 4 == 0)) { // 25% de chance
        g->bonus.actif = 1;
        g->bonus.en_attente = 0;
        g->bonus.x = save_x;
        g->bonus.y = save_y;
        g->bonus.type = (rand() % 2 == 0) ? TRIPLE_LASER : INVINCIBILITE;
    }
}

int asteroids_all_dead(const AsteroidManager *am) {
    return (am->spawned >= am->to_spawn) && (am->count == 0);
}