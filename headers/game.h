#ifndef PROJET_ALGO_S2_GAME_H
#define PROJET_ALGO_S2_GAME_H

#include <allegro.h>
#include "player.h"
#include "boss.h"
#include "asteroids.h"
#include "audio.h"

// Types de bonus que peut récupérer le joueur
typedef enum { AUCUN_BONUS, TRIPLE_LASER, INVINCIBILITE } TypeBonus;

// Bonus qui tombe au sol après destruction d'un astéroide
typedef struct {
    float x, y;
    TypeBonus type;
    int actif;         // 1 si le bonus est visible/ramassable
    int timer_vie;     // dispparait au bout d'un certain temps
    int en_attente;
} BonusDrop;

// les niveau du jeu chainés entre eux au suivant
typedef struct Level {
    int   n;             // numéro du niveau
    int   is_boss_level; // 1 si c'est le niveau du boss
    float gravity;
    int   max_asteroids;
    float max_size;
    struct Level *next_level;
} Level;

//Etat de la partie en cours

typedef struct Game {
    int nlevels;
    Level **levels;       // tableau de tous les niveaux chargés
    Level *current_level;
    int key_o_pressed;
    AsteroidManager am;
    BonusDrop bonus;
    int bonus_timer;
    TypeBonus bonus_actif; // bonus en cours d'utilisation par le joueur
    int level_complete;
} Game;

Game init_game(void);
void game_update(Player *player, Boss *boss, Game *game, Audio *audio);

// Gestion des collisions
void colision(Player *player, Boss *boss, Game *game);
void colision_eclair(Player *player, Boss *boss);
void colision_laser_asteroids(Player *player, Game *game, Audio *audio);

int  game_level_index(const Game *game);  // index du niveau actuel
int  game_is_boss(const Game *game);
int  game_ground_height(void);

// Bonus
void game_next_level(Game *game, Boss *boss);
void apply_bonus_effects(Game *g, Player *p);
void update_game_bonus(Game *g, Player *p);
void colision_asteroids_player(Player *p, AsteroidManager *am, Audio *audio);

// Spawn un power-up à la position donnée (même système que les niveaux normaux)
void spawn_boss_bonus(Game *g, float x, float y);

#endif
