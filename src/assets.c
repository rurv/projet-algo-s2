//
// Created by ruben on 4/2/2026.
//

#include "../headers/assets.h"

void load_assets (Assets *a) {
    // a->... = load_bitmap("assets/images/... .bmp", NULL);
    a->player_sprites = load_bitmap("assets/images/sprites_ships.bmp", NULL);
    a->laser_sprite = load_bitmap("assets/images/laser.bmp", NULL);
    a->boss = load_bitmap("assets/images/boss.bmp", NULL);
    a->explosion1 = load_bitmap("assets/images/explosion1.bmp", NULL);
    a->claude = load_bitmap("assets/images/claude.bmp", NULL);

    // if (!a->...) { ... }
    if (!a->player_sprites) {
        a->player_sprites = load_bitmap("../assets/images/sprites_ships.bmp", NULL);
        if (!a->player_sprites) {
            allegro_message("Erreur chargement player_sprites");
            allegro_exit();
            exit(1);
        }
    }
    if (!a->laser_sprite) {
        a->laser_sprite = load_bitmap("../assets/images/laser.bmp", NULL);
        if (!a->laser_sprite) {
            allegro_message("Erreur chargement laser_sprites");
            allegro_exit();
            exit(1);
        }
    }
    if (!a->boss) {
        a->boss = load_bitmap("../assets/images/boss.bmp", NULL);
        if (!a->boss) {
            allegro_message("Erreur chargement boss.bmp");
            allegro_exit();
            exit(1);
        }
    }
    if (!a->explosion1) {
        a->explosion1 = load_bitmap("../assets/images/explosion1.bmp", NULL);
        if (!a->explosion1) {
            allegro_message("Erreur chargement explosion1.bmp");
            allegro_exit();
            exit(1);
        }
    }
    if (!a->claude) {
        a->claude = load_bitmap("../assets/images/claude.bmp", NULL);
        if (!a->claude) {
            allegro_message("Erreur chargement claude.bmp");
            allegro_exit();
            exit(1);
        }
    }
}
    void free_assets (Assets *a) {
        // destroy_bitmap(a->...);
        destroy_bitmap(a->player_sprites);
        destroy_bitmap(a->laser_sprite);
        destroy_bitmap(a->boss);
        destroy_bitmap(a->explosion1);
        destroy_bitmap(a->claude);
    }
