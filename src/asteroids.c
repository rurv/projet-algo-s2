#include "../headers/asteroids.h"
#include "../headers/config.h"
#include <stdlib.h>
#include <math.h>

// Dessine un cercle marron plein (asset procédural)
static void draw_asteroid(BITMAP *buf, int x, int y, int r) {
    int dark  = makecol(101, 67, 33);
    int light = makecol(139, 90, 43);
    circlefill(buf, x, y, r,     dark);
    circlefill(buf, x - r/4, y - r/4, r * 2 / 3, light);
    circle(buf, x, y, r, makecol(70, 45, 20));
}

static int ground_y(void) {
    return SCREEN_HEIGHT - (SCREEN_HEIGHT - (SCREEN_HEIGHT / 5 * 4 + 84));
}

static void spawn_one(AsteroidManager *am) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (am->asteroids[i].active) continue;
        Asteroid *a = &am->asteroids[i];
        a->radius = am->max_size / 2 + rand() % (am->max_size / 2 + 1);
        a->x  = a->radius + rand() % (SCREEN_WIDTH  - 2 * a->radius);
        a->y  = a->radius + rand() % (SCREEN_HEIGHT / 3);
        a->dx = (float)(rand() % 5 + 1) * (rand() % 2 ? 1 : -1);
        a->dy = (float)(rand() % 3 + 1);
        a->active = 1;
        am->spawned++;
        am->count++;
        return;
    }
}

void asteroids_init(AsteroidManager *am, float gravity, int to_spawn, int max_size) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) am->asteroids[i].active = 0;
    am->count       = 0;
    am->spawned     = 0;
    am->to_spawn    = to_spawn;
    am->spawn_timer = 0;
    am->gravity     = gravity;   // pixels/s² * (1/FPS²) — appliqué chaque frame
    am->max_size    = max_size;
}

void asteroids_update(AsteroidManager *am) {
    // Spawn progressif : un nouvel astéroïde toutes les ~120 frames
    if (am->spawned < am->to_spawn) {
        am->spawn_timer++;
        if (am->spawn_timer >= 120) {
            am->spawn_timer = 0;
            spawn_one(am);
        }
    }

    float g = am->gravity / (60.0f * 60.0f); // converti en px/frame²

    int gy = ground_y();

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        Asteroid *a = &am->asteroids[i];
        if (!a->active) continue;

        a->dy += g;
        a->x  += a->dx;
        a->y  += a->dy;

        // Rebond sol (élastique parfait, pas de frottement)
        if (a->y + a->radius >= gy) {
            a->y  = gy - a->radius;
            a->dy = -a->dy;
        }
        // Rebond bords latéraux
        if (a->x - a->radius <= 0) {
            a->x  = a->radius;
            a->dx = -a->dx;
        } else if (a->x + a->radius >= SCREEN_WIDTH) {
            a->x  = SCREEN_WIDTH - a->radius;
            a->dx = -a->dx;
        }
        // Rebond plafond
        if (a->y - a->radius <= 0) {
            a->y  = a->radius;
            a->dy = -a->dy;
        }
    }
}

void asteroids_draw(BITMAP *buffer, AsteroidManager *am) {
    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        Asteroid *a = &am->asteroids[i];
        if (!a->active) continue;
        draw_asteroid(buffer, (int)a->x, (int)a->y, a->radius);
    }
}

void asteroid_split(AsteroidManager *am, int idx) {
    Asteroid *a = &am->asteroids[idx];
    if (!a->active) return;

    int new_r = a->radius / 2;
    a->active = 0;
    am->count--;

    if (new_r < MIN_ASTEROID_SIZE) return; // trop petit, disparait

    // Crée deux fragments
    for (int frag = 0; frag < 2; frag++) {
        for (int i = 0; i < MAX_ASTEROIDS; i++) {
            if (am->asteroids[i].active) continue;
            Asteroid *b = &am->asteroids[i];
            b->radius = new_r;
            b->x  = a->x + (frag == 0 ? -new_r : new_r);
            b->y  = a->y;
            b->dx = a->dx * (frag == 0 ? -1.2f : 1.2f);
            b->dy = a->dy - 1.0f;
            b->active = 1;
            am->count++;
            break;
        }
    }
}

int asteroids_all_dead(const AsteroidManager *am) {
    return (am->spawned >= am->to_spawn) && (am->count == 0);
}
