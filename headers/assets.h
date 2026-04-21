//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_ASSETS_H
#define PROJET_ALGO_S2_ASSETS_H

#include <allegro.h>

// structure qui stocke tous les assets du jeu
typedef struct Assets {
    BITMAP *bg1;
    BITMAP *bg2;
    BITMAP *bg3;
    BITMAP *bg4;
    BITMAP *bg5;
    BITMAP *player_sprites;
    BITMAP *asteroid_sprites;
    BITMAP *laser_sprite;
} Assets;

void load_assets (Assets *a);   // chargement des assets
void free_assets (Assets *a);   // liberation des assets

#endif //PROJET_ALGO_S2_ASSETS_H