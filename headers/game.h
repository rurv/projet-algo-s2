#ifndef PROJET_ALGO_S2_GAME_H
#define PROJET_ALGO_S2_GAME_H

#include <allegro.h>
#include "player.h"
#include "boss.h"
#include "asteroids.h"

typedef struct Level {
    int   n;
    int   is_boss_level;
    float gravity;
    int   max_asteroids;
    int   max_size;
    struct Level *next_level;
} Level;

typedef struct {
    int              nlevels;
    Level          **levels;
    Level           *current_level;
    int              key_o_pressed;
    AsteroidManager  am;
} Game;

Game init_game(void);
void game_update(Player *player, Boss *boss, Game *game);
void colision(Player *player, Boss *boss);
void colision_eclair(Player *player, Boss *boss);
void colision_laser_asteroids(Player *player, Game *game);

int  game_level_index(const Game *game);
int  game_is_boss(const Game *game);
int  game_ground_height(void);
void game_next_level(Game *game, Boss *boss);

#endif
