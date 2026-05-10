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
        asteroids_init(&game->am, 0.0f, 0, 0.0);
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
        if (fscanf(f, "%d %d %f %d %f",
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

    game.levels         = levels;
    game.nlevels        = nlevels;
    game.current_level  = levels[0];
    game.bonus.actif    = 0;
    game.bonus_timer    = 0;
    game.level_complete = 0;

    load_level_asteroids(&game);
    return game;
}

void game_update(Player *player, Boss *boss, Game *game, Audio *audio) {
    player_update(player);
    update_game_bonus(game, player);
    apply_bonus_effects(game, player);

    if (player->invincible_timer <= 0 && game->bonus_timer <= 0) {
        player->invincible = 0;
    } else if (player->invincible_timer <= 0 && game->bonus_actif == TRIPLE_LASER) {
        player->invincible = 0;
    } else {
        player->invincible = 1;
    }

    if (!game_is_boss(game)) {
        asteroids_update(&game->am);
        colision_laser_asteroids(player, game, audio);

        if (asteroids_all_dead(&game->am) && !game->level_complete)
            game->level_complete = 1;
    } else {
        // Ne pas mettre à jour le boss s'il est déjà mort ou en train de mourir
        if (boss->active && boss->pv > 0 && !boss->dying)
            boss_update(boss);
        else if (boss->dying)
            boss_update(boss);   // boss_update gère la séquence de mort

        colision(player, boss, game);
        colision_eclair(player, boss);

        static int eclair_jouait = 0;

        if (boss->eclair_active) {
            if (!eclair_jouait) {
                audio_play_boss_eclair(audio);
                eclair_jouait = 1;
            }
        } else {
            if (eclair_jouait) {
                audio_stop_boss_eclair(audio);
                eclair_jouait = 0;
            }
        }
        if (boss->pv <= 0 && !boss->dying && !boss->death_done) {
            if (eclair_jouait) {
                audio_stop_boss_eclair(audio);
                eclair_jouait = 0;
            }
        }
    }

    if (key[KEY_LEFT])  player_move_left(player);
    if (key[KEY_RIGHT]) player_move_right(player);

    if (key[KEY_O]) {
        if (!game->key_o_pressed) {
            game_next_level(game, boss);
            game->key_o_pressed = 1;
        }
    } else {
        game->key_o_pressed = 0;
    }
}

// Spawn un power-up à la position donnée si aucun n'est déjà actif
void spawn_boss_bonus(Game *g, float x, float y) {
    if (g->bonus.actif) return;
    g->bonus.actif      = 1;
    g->bonus.en_attente = 0;
    g->bonus.x          = x;
    g->bonus.y          = y;
    g->bonus.type       = (rand() % 2 == 0) ? TRIPLE_LASER : INVINCIBILITE;
}

// Collision laser/boss avec 5% de chance de power-up à chaque touche
void colision(Player *player, Boss *boss, Game *game) {
    for (int i = 0; i < player->laser_count; i++) {
        if (!player->lasers[i].active) continue;
        if (player->lasers[i].x >= boss->x &&
            player->lasers[i].y >= boss->y &&
            player->lasers[i].x <= boss->x + 300 &&
            player->lasers[i].y <= boss->y + 185) {
            player->lasers[i].active = 0;
            boss->pv -= 10;
            if (boss->pv <= 0) boss->pv = 0;

            // 5% de chance de faire tomber un power-up
            if (rand() % 100 < 5)
                spawn_boss_bonus(game, boss->x + rand() % 280 + 10, boss->y + 100);
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
    int p_left    = player->x + 15, p_right = player->x + 45;
    if (player->y >= 160) {
        if ((p_right >= eclair_x1 - 15 && p_left <= eclair_x1 + 15) ||
            (p_right >= eclair_x2 - 15 && p_left <= eclair_x2 + 15)) {
            player->vies--;
            if (player->vies < 0) player->vies = 0;
            player->invincible       = 1;
            player->invincible_timer = 120;
        }
    }
}

void colision_laser_asteroids(Player *player, Game *game, Audio *audio) {
    AsteroidManager *am = &game->am;
    for (int l = 0; l < player->laser_count; l++) {
        if (!player->lasers[l].active) continue;
        float lx = player->lasers[l].x, ly = player->lasers[l].y;
        for (int a = 0; a < MAX_ASTEROIDS; a++) {
            if (!am->asteroids[a].active) continue;
            float dx    = lx - am->asteroids[a].x;
            float dy    = ly - am->asteroids[a].y;
            float dist2 = dx * dx + dy * dy;
            float r     = am->asteroids[a].radius;
            if (dist2 <= r * r) {
                player->lasers[l].active = 0;
                asteroid_split(am, a, game);
                audio_play_hit_laser(audio);
                break;
            }
        }
    }
}

void colision_asteroids_player(Player *p, AsteroidManager *am, Audio *audio) {
    float p_centerX = p->x + 30;
    float p_centerY = p->y + 40;
    float p_radius  = 25.0f;

    for (int i = 0; i < MAX_ASTEROIDS; i++) {
        if (!am->asteroids[i].active) continue;
        Asteroid *a = &am->asteroids[i];
        float dx                = p_centerX - a->x;
        float dy                = p_centerY - a->y;
        float distance_au_carre = dx * dx + dy * dy;
        float somme_rayons      = p_radius + a->radius;
        float seuil_collision   = somme_rayons * somme_rayons;

        if (distance_au_carre < seuil_collision) {
            if (!p->invincible) {
                p->vies--;
                audio_play_hit(audio);
                p->invincible       = 1;
                p->invincible_timer = 120;
            } else {
                audio_play_hit_laser(audio);
            }
            a->active = 0;
            am->count--;
        }
    }
}

void update_game_bonus(Game *g, Player *p) {
    if (!g->bonus.actif) return;

    if (!g->bonus.en_attente) {
        g->bonus.y += 3.0f;
        if (g->bonus.y >= p->y + 60) {
            g->bonus.y        = p->y + 60;
            g->bonus.en_attente = 1;
            g->bonus.timer_vie  = 180;
        }
    } else {
        g->bonus.timer_vie--;
        if (g->bonus.timer_vie <= 0) g->bonus.actif = 0;
    }

    float dist_x = abs(g->bonus.x - (p->x + 30));
    float dist_y = abs(g->bonus.y - (p->y + 30));
    if (dist_x < 50 && dist_y < 50) {
        g->bonus_actif = g->bonus.type;
        if (g->bonus_actif == INVINCIBILITE) {
            p->invincible  = 1;
            g->bonus_timer = 600;
        }
        if (g->bonus_actif == TRIPLE_LASER) {
            g->bonus_timer       = 200;
            p->invincible        = 0;
            p->invincible_timer  = 0;
        }
        g->bonus.actif = 0;
    }
}

void apply_bonus_effects(Game *g, Player *p) {
    if (g->bonus_timer > 0) {
        g->bonus_timer--;

        if (g->bonus_actif == TRIPLE_LASER && key[KEY_SPACE]) {
            int tirs_faits = 0;
            for (int i = 0; i < p->laser_count && tirs_faits < 2; i++) {
                if (!p->lasers[i].active) {
                    p->lasers[i].x      = p->x + 24;
                    p->lasers[i].y      = p->y;
                    p->lasers[i].dy     = -25.0f;
                    p->lasers[i].dx     = (tirs_faits == 0) ? -5.0f : 5.0f;
                    p->lasers[i].active = 1;
                    tirs_faits++;
                }
            }
        }
    } else {
        if (g->bonus_actif == INVINCIBILITE) p->invincible = 0;
        g->bonus_actif = AUCUN_BONUS;
    }
}
