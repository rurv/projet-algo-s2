#ifndef PROJET_ALGO_S2_GAME_H
#define PROJET_ALGO_S2_GAME_H

#include <allegro.h>
#include "player.h"
#include "boss.h"
#include "asteroids.h"

typedef enum { AUCUN_BONUS, TRIPLE_LASER, INVINCIBILITE } TypeBonus;

typedef struct {
    float x, y;
    TypeBonus type;
    int actif;       // 1 si la boule est à l'écran
    int timer_vie;   // Temps avant disparition (3 secondes)
    int en_attente;  // 1 si elle a fini de descendre et attend d'être ramassée
} BonusDrop;

typedef struct Level {
    int   n;
    int   is_boss_level;
    float gravity;
    int   max_asteroids;
    float   max_size;
    struct Level *next_level;
} Level;

typedef struct Game{
    int nlevels;
    Level **levels;
    Level *current_level;
    int key_o_pressed;
    AsteroidManager  am;
    BonusDrop bonus;
    int bonus_timer;
    TypeBonus bonus_actif;
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
void apply_bonus_effects(Game *g, Player *p);
void update_game_bonus(Game *g, Player *p);
void colision_asteroids_player(Player *p, AsteroidManager *am);

#endif
