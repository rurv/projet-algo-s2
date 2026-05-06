#ifndef PROJET_ALGO_S2_ASTEROIDS_H
#define PROJET_ALGO_S2_ASTEROIDS_H

#include <allegro.h>
#include "assets.h"

#define MAX_ASTEROIDS   32
#define MIN_ASTEROID_SIZE 20

typedef struct Asteroid {
    float x, y;
    float dx, dy;
    float radius;
    int   active;
    int   frame, frame_timer;
} Asteroid;

typedef struct AsteroidManager {
    Asteroid asteroids[MAX_ASTEROIDS];
    int     count;        // actifs + à spawner
    int     spawned;      // combien ont été créés
    int     to_spawn;     // total à spawner ce niveau
    int     spawn_timer;
    float   gravity;
    float   max_size;
} AsteroidManager;

void  asteroids_init(AsteroidManager *am, float gravity, int to_spawn, float max_size);
void  asteroids_update(AsteroidManager *am);
void  asteroids_draw(Assets as, BITMAP *buffer, AsteroidManager *am);
void  asteroid_split(AsteroidManager *am, int idx);
int   asteroids_all_dead(const AsteroidManager *am);

#endif
