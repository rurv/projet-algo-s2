
#ifndef PROJET_ALGO_S2_ASSETS_H
#define PROJET_ALGO_S2_ASSETS_H

#include <allegro.h>

// Nombre de sprites d'astéroïdes (un par niveau non-boss)
#define NB_ASTEROID_SPRITES 3

// structure qui stocke tous les assets du jeu
typedef struct Assets {
    BITMAP *bg1;
    BITMAP *bg2;
    BITMAP *bg3;
    BITMAP *bg4;
    BITMAP *bg5;
    BITMAP *player_sprites;
    BITMAP *asteroid_sprites[NB_ASTEROID_SPRITES];
    BITMAP *laser_sprite;
    BITMAP *boss;
    BITMAP *explosion1;
    BITMAP *claude;
    BITMAP *sol_sprites[3];
} Assets;



void load_assets (Assets *a);   // chargement des assets
void free_assets (Assets *a);   // liberation des assets

#endif //PROJET_ALGO_S2_ASSETS_H