#ifndef PROJET_ALGO_S2_DISPLAY_H
#define PROJET_ALGO_S2_DISPLAY_H

#include <allegro.h>
#include "config.h"
#include "assets.h"
#include "player.h"
#include "boss.h"
#include "game.h"

typedef struct {
    int x, y;
    int vitesse;
    int luminosite;
} Etoile;

enum SkinActuelle { VAISSEAU1, VAISSEAU2, VAISSEAU3, VAISSEAU4 };

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

// ── Cinématique de victoire ──────────────────────────────────────────────────
typedef enum {
    VICTORY_INACTIVE = 0,
    VICTORY_BOSS_DYING,      // tremblement du boss + mini-explosions (géré dans boss.c)
    VICTORY_BOSS_EXPLODING,  // grande explosion finale du boss
    VICTORY_WARP_CENTER,     // vaisseau se centre
    VICTORY_WARP_ACCEL,      // accélération des étoiles
    VICTORY_DIALOGUE,        // dialogue avec Claude
    VICTORY_SHIP_EXIT,       // vaisseau quitte l'écran vers le haut
    VICTORY_SCREEN,          // écran de victoire final
} VictoryPhase;

typedef struct {
    VictoryPhase phase;
    int  timer;
    float star_speed;
    int  dialogue_done;     // 1 quand le joueur a cliqué sur le dialogue
} VictoryCinematic;

typedef struct {
    int  active;
    int  done;
    int  timer;
    int  char_shown;
    int  phase;        // phase du dialogue (comme DiferrentDecompte)
    int  premier_affichage;
} GameOverDialogue;

// Rendu normal
void initialisation_allegro(void);
void init_display(Bitmaps *b, Assets *assets);
void display(Bitmaps *b, Assets *assets, Player *p, Boss *boss, const Game *game);
void display_eclair(Bitmaps *b, Boss *boss, int active, int boss_active);
void display_hud(Bitmaps *b, Player *p, Boss *boss, const Game *game);
void destroy_display(Bitmaps *b, Assets *assets);
void draw_bonus_system(BITMAP *buffer, Game *g, Assets *as);
void draw_neon_circle(BITMAP *dest, int x, int y, int radius);

// Cinématique inter-niveaux
void transition_reset(Transition *tr);
void transition_start(Transition *tr);
int  transition_update(Transition *tr, Player *p, Game *game, Boss *boss,
                       int *boss_dead_sound, float player_game_y);
void display_transition(Transition *tr, Bitmaps *b, Assets *assets,
                        Player *p, Game *game, Etoile *etoiles, int n_etoiles);
void transition_update_etoiles(Transition *tr, Etoile *etoiles, int n_etoiles);

// Cinématique de victoire
void victory_cinematic_reset(VictoryCinematic *vc);
void victory_cinematic_start(VictoryCinematic *vc);
int  victory_cinematic_update(VictoryCinematic *vc, Player *p, Boss *boss,
                               Etoile *etoiles, int n_etoiles, float player_game_y);
void victory_cinematic_draw(VictoryCinematic *vc, Bitmaps *b, Assets *assets,
                            Player *p, Boss *boss, Etoile *etoiles, int n_etoiles);

// Dialogue Game Over
void gameover_dialogue_reset(GameOverDialogue *d);
int  gameover_dialogue_update(GameOverDialogue *d);
void gameover_dialogue_draw(BITMAP *buffer, GameOverDialogue *d, Assets *assets, Player *p, Audio *audio);

#endif
