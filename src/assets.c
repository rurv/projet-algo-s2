//
// Created by ruben on 4/2/2026.
//

#include "../headers/assets.h"

void load_assets (Assets *a) {
    // a->... = load_bitmap("assets/images/... .bmp", NULL);
    a->player_sprites = load_bitmap("../assets/images/sprites_ships.bmp", NULL);

    // if (!a->...) { ... }
    if (!a->player_sprites) {
        allegro_message("Erreur chargement player_sprites");
        allegro_exit();
        exit(1);
    }
}

void free_assets (Assets *a) {
    // destroy_bitmap(a->...);
    destroy_bitmap(a->player_sprites);
}

