
#include "../headers/assets.h"
#include <stdio.h>

// Chemins des sprites d'astéroïdes (un par niveau)
static const char *asteroid_paths[NB_ASTEROID_SPRITES] = {
    "assets/images/sprites_asteroid.bmp",
    "assets/images/sprites_asteroid2.bmp",
    "assets/images/sprites_asteroid3.bmp"
};
// Chemins de fallback (depuis le dossier de build)
static const char *asteroid_paths_fb[NB_ASTEROID_SPRITES] = {
    "../assets/images/sprites_asteroid.bmp",
    "../assets/images/sprites_asteroid2.bmp",
    "../assets/images/sprites_asteroid3.bmp"
};


void load_assets (Assets *a) {
    a->player_sprites = load_bitmap("assets/images/sprites_ships.bmp", NULL);
    a->laser_sprite   = load_bitmap("assets/images/laser.bmp", NULL);
    a->boss           = load_bitmap("assets/images/boss.bmp", NULL);
    a->explosion1     = load_bitmap("assets/images/explosion1.bmp", NULL);
    a->claude         = load_bitmap("assets/images/claude.bmp", NULL);
    a->sol_sprites[0] = load_bitmap("../assets/images/sol_lune.bmp", NULL);
    a->sol_sprites[1] = load_bitmap("../assets/images/sol_volcan.bmp", NULL);
    a->sol_sprites[2] = load_bitmap("../assets/images/sol_cristal.bmp", NULL);

    // Chargement des sprites d'astéroïdes (1 par niveau)
    for (int i = 0; i < NB_ASTEROID_SPRITES; i++) {
        a->asteroid_sprites[i] = load_bitmap(asteroid_paths[i], NULL);
    }

    if (!a->player_sprites) {
        a->player_sprites = load_bitmap("../assets/images/sprites_ships.bmp", NULL);
        if (!a->player_sprites) {
            allegro_message("Erreur chargement player_sprites");
            allegro_exit(); exit(1);
        }
    }
    if (!a->laser_sprite) {
        a->laser_sprite = load_bitmap("../assets/images/laser.bmp", NULL);
        if (!a->laser_sprite) {
            allegro_message("Erreur chargement laser_sprites");
            allegro_exit(); exit(1);
        }
    }
    if (!a->boss) {
        a->boss = load_bitmap("../assets/images/boss.bmp", NULL);
        if (!a->boss) {
            allegro_message("Erreur chargement boss.bmp");
            allegro_exit(); exit(1);
        }
    }
    if (!a->explosion1) {
        a->explosion1 = load_bitmap("../assets/images/explosion1.bmp", NULL);
        if (!a->explosion1) {
            allegro_message("Erreur chargement explosion1.bmp");
            allegro_exit(); exit(1);
        }
    }
    if (!a->claude) {
        a->claude = load_bitmap("../assets/images/claude.bmp", NULL);
        if (!a->claude) {
            allegro_message("Erreur chargement claude.bmp");
            allegro_exit(); exit(1);
        }
    }

    if (!a->sol_sprites[0]) {
        a->sol_sprites[0] = load_bitmap("../assets/images/sol_lune.bmp", NULL);
        if (!a->sol_sprites[0]) { allegro_message("Erreur chargement sol_lune.bmp"); allegro_exit(); exit(1); }
    }
    if (!a->sol_sprites[1]) {
        a->sol_sprites[1] = load_bitmap("../assets/images/sol_volcan.bmp", NULL);
        if (!a->sol_sprites[1]) { allegro_message("Erreur chargement sol_volcan.bmp"); allegro_exit(); exit(1); }
    }
    if (!a->sol_sprites[2]) {
        a->sol_sprites[2] = load_bitmap("../assets/images/sol_cristal.bmp", NULL);
        if (!a->sol_sprites[2]) { allegro_message("Erreur chargement sol_cristal.bmp"); allegro_exit(); exit(1); }
    }

    // Fallback pour chaque sprite d'astéroïde
    for (int i = 0; i < NB_ASTEROID_SPRITES; i++) {
        if (!a->asteroid_sprites[i]) {
            a->asteroid_sprites[i] = load_bitmap(asteroid_paths_fb[i], NULL);
            if (!a->asteroid_sprites[i]) {
                char msg[128];
                snprintf(msg, sizeof(msg), "Erreur chargement %s", asteroid_paths[i]);
                allegro_message(msg);
                allegro_exit(); exit(1);
            }
        }
    }
}

void free_assets (Assets *a) {
    destroy_bitmap(a->player_sprites);
    destroy_bitmap(a->laser_sprite);
    destroy_bitmap(a->boss);
    destroy_bitmap(a->explosion1);
    destroy_bitmap(a->claude);
    for (int i = 0; i < NB_ASTEROID_SPRITES; i++) {
        destroy_bitmap(a->asteroid_sprites[i]);
    }
    for (int i = 0; i < 3; i++)
        destroy_bitmap(a->sol_sprites[i]);
}