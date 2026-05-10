#ifndef PROJET_ALGO_S2_ASTEROIDS_H
#define PROJET_ALGO_S2_ASTEROIDS_H

#include <allegro.h>

#define MAX_ASTEROIDS   32
#define MIN_ASTEROID_SIZE 20

typedef struct Game Game;

// structure des asteroides
typedef struct Asteroid {
    float x, y;
    float dx, dy;
    float radius;
    int   active;
    int   frame, frame_timer;
} Asteroid;

// structure de gestion des asteroides
typedef struct AsteroidManager {
    Asteroid asteroids[MAX_ASTEROIDS];
    int     count;
    int     spawned;
    int     to_spawn;
    int     spawn_timer;
    float   gravity;
    float   max_size;
} AsteroidManager;

// calcul de la hauteur du sol en fonction des dimensions de l'ecran
int     ground_y(void);
void    draw_asteroid(BITMAP *sprite, BITMAP *buf, float cx, float cy, float r, int frame); // calcul des dimensions des asteroides
void    spawn_one(AsteroidManager *am);                                                     // spawn d'un asteroide
void    asteroids_init(AsteroidManager *am, float gravity, int to_spawn, float max_size);   // initialisation des asteroides
void    asteroids_update(AsteroidManager *am);                                              // mise à jour des asteroides
void    asteroids_draw(BITMAP *sprite, BITMAP *buffer, AsteroidManager *am);                // affichage des asteroides
void    asteroid_split(AsteroidManager *am, int idx, Game *g);                              // division des asteroides
int     asteroids_all_dead(const AsteroidManager *am);                                      // verification de la condition de victoire

#endif