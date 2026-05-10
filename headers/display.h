#ifndef PROJET_ALGO_S2_DISPLAY_H
#define PROJET_ALGO_S2_DISPLAY_H

#include <allegro.h>
#include "config.h"
#include "assets.h"
#include "player.h"
#include "boss.h"
#include "game.h"

// Etoile est définie ici car utilisée à la fois par le rendu (display)
// et par la boucle principale (main). ihm.h ne l'inclut plus.
typedef struct {
    int x, y;
    int vitesse;
    int luminosite;
} Etoile;

enum SkinActuelle {VAISSEAU1, VAISSEAU2, VAISSEAU3, VAISSEAU4};

typedef struct Bitmaps {
    BITMAP *buffer;
    BITMAP *ship;
    BITMAP *fond;
    BITMAP *asteroid;
    float   fond_scroll_x;
} Bitmaps;

// ── Cinématique inter-niveaux ────────────────────────────────────────────────
typedef enum {
    TRANS_INACTIVE = 0,
    TRANS_CENTER,
    TRANS_ACCEL,
    TRANS_WARP,
    TRANS_ARRIVE,
    TRANS_DESCEND,
} TransPhase;

typedef struct {
    TransPhase phase;
    int        timer;
    float      star_speed;
    float      sol_offset;
    float      ship_anim_y;
    int        level_changed;
} Transition;

#define TRANS_CENTER_DUR  60
#define TRANS_ACCEL_DUR   90
#define TRANS_WARP_DUR    80
#define TRANS_ARRIVE_DUR  70
#define TRANS_DESCEND_DUR 80

// Rendu normal
void initialisation_allegro(void);
void init_display(Bitmaps *b, Assets *assets);
void display(Bitmaps *b, Assets *assets, Player *p, Boss *boss, const Game *game);
void display_eclair(Bitmaps *b, Boss *boss, int active, int boss_active);
void display_hud(Bitmaps *b, Player *p, Boss *boss, const Game *game);
void destroy_display(Bitmaps *b, Assets *assets);
void draw_bonus_system(BITMAP *buffer, Game *g, Assets *as);
void draw_neon_circle(BITMAP *dest, int x, int y, int radius);

// Cinématique
void transition_reset(Transition *tr);
void transition_start(Transition *tr);
int  transition_update(Transition *tr, Player *p, Game *game, Boss *boss,
                       int *boss_dead_sound, float player_game_y);
void display_transition(Transition *tr, Bitmaps *b, Assets *assets,
                        Player *p, Game *game, Etoile *etoiles, int n_etoiles);
void transition_update_etoiles(Transition *tr, Etoile *etoiles, int n_etoiles);

#endif