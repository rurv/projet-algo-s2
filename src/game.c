//
// Created by ruben on 4/2/2026.
//

#include "../headers/game.h"

#include "../headers/audio.h"

void game_update (Player *player, Boss *boss) {
    player_update (player);
    boss_update (boss);

    if (key[KEY_LEFT]) player_move_left(player);
    if (key[KEY_RIGHT]) player_move_right(player);
    colision(player, boss);
}

void colision(Player *player, Boss *boss) {
    for (int i = 0; i < player->laser_count; i++){
        if (!player->lasers[i].active) continue;
        if (player->lasers[i].x >= boss->x && player->lasers[i].y >= boss->y && player->lasers[i].x <= boss->x + 300 && player->lasers[i].y <= boss->y + 185) {
        player->lasers[i].active = 0;
        boss->pv -= 10;
        if (boss->pv <= 0) boss->pv = 0;
        }
    }
}