//
// Created by ruben on 4/2/2026.
//

#include "../headers/display.h"

void initialisation_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());
    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED,SCREEN_WIDTH,SCREEN_HEIGHT,0,0)!=0) {
        allegro_message("probleme mode graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    show_mouse(screen);
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

BITMAP *player_sprite (Assets assets) {
    BITMAP *player_sprite = create_bitmap(32, 32);
    masked_blit(assets.player_sprites, player_sprite, 80, 320, 0, 0, player_sprite->w, player_sprite->h);
    return player_sprite;
}

void display (BITMAP *buffer, Assets *assets, Player *player) {
    clear_bitmap(buffer);
    draw_player(&buffer, *assets, *player);
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void destroy_display (BITMAP **buffer, Assets *assets) {
    free_assets(assets);
    destroy_bitmap(*buffer);
}