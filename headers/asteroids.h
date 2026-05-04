#ifndef PROJET_ALGO_S2_ASTEROIDS_H
#define PROJET_ALGO_S2_ASTEROIDS_H

#include <allegro.h>

#define MAX_ASTEROIDS   32
#define MIN_ASTEROID_SIZE 15

typedef struct Asteroid {
    float x, y;
    float dx, dy;
    int   radius;
    int   active;
} Asteroid;

typedef struct AsteroidManager {
    Asteroid asteroids[MAX_ASTEROIDS];
    int      count;        // actifs + à spawner
    int      spawned;      // combien ont été créés
    int      to_spawn;     // total à spawner ce niveau
    int      spawn_timer;
    float    gravity;
    int      max_size;
} AsteroidManager;

void  asteroids_init(AsteroidManager *am, float gravity, int to_spawn, int max_size);
void  asteroids_update(AsteroidManager *am);
void  asteroids_draw(BITMAP *buffer, AsteroidManager *am);
void  asteroid_split(AsteroidManager *am, int idx);
int   asteroids_all_dead(const AsteroidManager *am);

#endif
