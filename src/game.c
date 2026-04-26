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
    colision_eclair(player, boss);
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
void colision_eclair(Player *player, Boss *boss) {
    if (!boss->eclair_active || !boss->active) return;

    if (player->invincible) {
        player->invincible_timer--;
        if (player->invincible_timer <= 0) player->invincible = 0;
        return;
    }

    // Les deux éclairs sont à boss->x + 100 et boss->x + 200, largeur ~15px
    int eclair_x1 = boss->x + 100;
    int eclair_x2 = boss->x + 200;
    int p_left  = player->x + 15;  // on ignore 15px à gauche
    int p_right = player->x + 45;  // on ignore 15px à droite
    int py = player->y;

    if (py >= 160) { // l'éclair commence à y=160
        if ((p_right >= eclair_x1 - 15 && p_left <= eclair_x1 + 15) ||
            (p_right >= eclair_x2 - 15 && p_left <= eclair_x2 + 15) &&
            !player->invincible) {
            player->vies--;
            if (player->vies < 0) player->vies = 0;
            player->invincible = 1;
            player->invincible_timer = 120; // 2 sec d'invincibilité
            }
    }
}