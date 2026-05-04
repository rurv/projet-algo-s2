#include "../headers/game.h"
#include "../headers/config.h"
#include "../headers/audio.h"
#include <stdio.h>
#include <stdlib.h>

int game_level_index(const Game *game) {
    for (int i = 0; i < game->nlevels; i++)
        if (game->levels[i] == game->current_level) return i;
    return 0;
}

int game_is_boss(const Game *game) {
    return game->current_level && game->current_level->is_boss_level;
}

int game_ground_height(void) {
    return SCREEN_HEIGHT - (SCREEN_HEIGHT / 5 * 4 + 84);
}

static void load_level_asteroids(Game *game) {
    Level *lv = game->current_level;
    if (lv->is_boss_level)
        asteroids_init(&game->am, 0.0f, 0, 0);
    else
        asteroids_init(&game->am, lv->gravity, lv->max_asteroids, lv->max_size);
}

void game_next_level(Game *game, Boss *boss) {
    if (game->current_level && game->current_level->next_level) {
        game->current_level = game->current_level->next_level;
        load_level_asteroids(game);
        if (game_is_boss(game)) {
            boss->active = 1;
            boss->pv     = BOSS_MAX_PV;
        }
    }
}

Game init_game(void) {
    Game game;
    game.key_o_pressed = 0;

    FILE *f = fopen("data/levels.txt", "r");
    if (!f) f = fopen("../data/levels.txt", "r");
    if (!f) { allegro_message("Erreur ouverture levels.txt"); allegro_exit(); exit(1); }

    int nlevels;
    if (fscanf(f, "%d", &nlevels) != 1 || nlevels <= 0) {
        allegro_message("Erreur lecture nlevels"); fclose(f); allegro_exit(); exit(1);
    }

    Level **levels = malloc(nlevels * sizeof(Level *));
    if (!levels) { allegro_message("Erreur malloc levels"); fclose(f); allegro_exit(); exit(1); }

    for (int i = 0; i < nlevels; i++) {
        levels[i] = malloc(sizeof(Level));
        if (!levels[i]) { allegro_message("Erreur malloc level"); fclose(f); allegro_exit(); exit(1); }
        if (fscanf(f, "%d %d %f %d %d",
                   &levels[i]->n,
                   &levels[i]->is_boss_level,
                   &levels[i]->gravity,
                   &levels[i]->max_asteroids,
                   &levels[i]->max_size) != 5) {
            allegro_message("Erreur lecture niveau %d", i); fclose(f); allegro_exit(); exit(1);
        }
        levels[i]->next_level = NULL;
    }
    fclose(f);

    for (int i = 0; i < nlevels - 1; i++)
        levels[i]->next_level = levels[i + 1];

    game.levels        = levels;
    game.nlevels       = nlevels;
    game.current_level = levels[0];

    load_level_asteroids(&game);
    return game;
}

void game_update(Player *player, Boss *boss, Game *game) {
    player_update(player);

    if (!game_is_boss(game)) {
        asteroids_update(&game->am);
        colision_laser_asteroids(player, game);

        // Passage automatique quand tous les astéroïdes sont détruits
        if (asteroids_all_dead(&game->am))
            game_next_level(game, boss);
    } else {
        boss_update(boss);
        colision(player, boss);
        colision_eclair(player, boss);
    }

    if (key[KEY_LEFT])  player_move_left(player);
    if (key[KEY_RIGHT]) player_move_right(player);

    // Touche O : niveau suivant (debug, anti-répétition)
    if (key[KEY_O]) {
        if (!game->key_o_pressed) {
            game_next_level(game, boss);
            game->key_o_pressed = 1;
        }
    } else {
        game->key_o_pressed = 0;
    }
}

void colision(Player *player, Boss *boss) {
    for (int i = 0; i < player->laser_count; i++) {
        if (!player->lasers[i].active) continue;
        if (player->lasers[i].x >= boss->x &&
            player->lasers[i].y >= boss->y &&
            player->lasers[i].x <= boss->x + 300 &&
            player->lasers[i].y <= boss->y + 185) {
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
    int eclair_x1 = boss->x + 100, eclair_x2 = boss->x + 200;
    int p_left = player->x + 15, p_right = player->x + 45;
    if (player->y >= 160) {
        if ((p_right >= eclair_x1 - 15 && p_left <= eclair_x1 + 15) ||
            (p_right >= eclair_x2 - 15 && p_left <= eclair_x2 + 15)) {
            player->vies--;
            if (player->vies < 0) player->vies = 0;
            player->invincible = 1;
            player->invincible_timer = 120;
        }
    }
}

void colision_laser_asteroids(Player *player, Game *game) {
    AsteroidManager *am = &game->am;
    for (int l = 0; l < player->laser_count; l++) {
        if (!player->lasers[l].active) continue;
        float lx = player->lasers[l].x, ly = player->lasers[l].y;
        for (int a = 0; a < MAX_ASTEROIDS; a++) {
            if (!am->asteroids[a].active) continue;
            float dx = lx - am->asteroids[a].x;
            float dy = ly - am->asteroids[a].y;
            float dist2 = dx*dx + dy*dy;
            float r = am->asteroids[a].radius;
            if (dist2 <= r * r) {
                player->lasers[l].active = 0;
                asteroid_split(am, a);
                break;
            }
        }
    }
}
