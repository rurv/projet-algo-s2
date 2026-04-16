//
// Created by ruben on 4/2/2026.
//

#include "../headers/game.h"

void game_update (Player *player) {
    player_update (player);

    if (key[KEY_LEFT]) player_move_left(player);
    if (key[KEY_RIGHT]) player_move_right(player);
}
