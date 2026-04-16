//
// Created by ruben on 4/2/2026.
//

#include "../headers/display.h"
#include "../headers/assets.h"

void initialisation_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());
    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED,960,540,0,0)!=0) {
        allegro_message("probleme mode graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
}

void init_display (BITMAP **buffer, Assets *assets) {
    initialisation_allegro();
    *buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!*buffer) {
        allegro_message("Erreur création buffer");
        allegro_exit();
        exit(1);
    }
    load_assets(assets);
}

void display (BITMAP *buffer, Assets *assets) {
    clear_bitmap(buffer);
    masked_blit(assets->player_sprites, buffer, 152, 336, SCREEN_W/3+100, 425, 48, 64);
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void destroy_display (BITMAP **buffer, Assets *assets) {
    free_assets(assets);
    destroy_bitmap(*buffer);
}