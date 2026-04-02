//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_ASSETS_H
#define PROJET_ALGO_S2_ASSETS_H

#include <allegro/gfx.h>

typedef struct Assets {
    BITMAP *bg1;
    BITMAP *bg2;
    BITMAP *bg3;
    BITMAP *bg4;
    BITMAP *bg5;
    BITMAP *player_sprites;
    BITMAP *ship_sprites;
} Assets;

void load_assets (Assets *a);
void free_assets (Assets *a);

#endif //PROJET_ALGO_S2_ASSETS_H