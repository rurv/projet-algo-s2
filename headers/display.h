#ifndef PROJET_ALGO_S2_DISPLAY_H
#define PROJET_ALGO_S2_DISPLAY_H

#include <allegro.h>
#include "config.h"
#include "assets.h"
#include "player.h"
#include "boss.h"
#include "game.h"
#include "ihm.h"

enum SkinActuelle {VAISSEAU1,VAISSEAU2,VAISSEAU3,VAISSEAU4};

typedef struct Bitmaps {
    BITMAP *buffer;
    BITMAP *ship;
    BITMAP *fond;
    BITMAP *asteroid;
    float   fond_scroll_x;
}Bitmaps;

void    initialisation_allegro(void);
void    init_display(Bitmaps *b, Assets *assets);
void    display(Bitmaps *b, Assets *assets, Player *p, Boss *boss, const Game *game);
void    display_eclair(Bitmaps *b, Boss *boss, int active, int boss_active);
void    display_hud(Bitmaps *b, Player *p, Boss *boss, const Game *game);
void    destroy_display(Bitmaps *b, Assets *assets);
void    draw_bonus_system(BITMAP *buffer, Game *g, Assets *as);
void    draw_neon_circle(BITMAP *dest, int x, int y, int radius);

#endif
