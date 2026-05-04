//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_GAME_H
#define PROJET_ALGO_S2_GAME_H

#include <allegro.h>
#include "player.h"
#include "boss.h"

typedef struct Level{
    int n;
    int is_boss_level;
    struct Level* next_level;
} Level;

typedef struct {
    int nlevels;
    Level** levels;
    Level* current_level;
} Game;


Level create_level(int n, int is_boss_level);
Game init_game (void);
void game_update (Player *player, Boss *boss);
void colision(Player *player, Boss *boss);
void colision_eclair(Player *player, Boss *boss);

#endif //PROJET_ALGO_S2_GAME_H