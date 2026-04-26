//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_GAME_H
#define PROJET_ALGO_S2_GAME_H

#include <allegro.h>
#include "player.h"
#include "boss.h"

void game_update (Player *player, Boss *boss);
void colision(Player *player, Boss *boss);
void colision_eclair(Player *player, Boss *boss);

#endif //PROJET_ALGO_S2_GAME_H