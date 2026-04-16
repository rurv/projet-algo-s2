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

void init_display (Bitmaps *b, Assets *assets) {
    initialisation_allegro();
    b->buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!b->buffer) {
        allegro_message("Erreur création buffer");
        allegro_exit();
        exit(1);
    }
    load_assets(assets);
    b->fond = create_bitmap(b->buffer->w,b->buffer->h);
    clear_to_color(b->fond, makecol(255, 0, 255));
    BITMAP *etoile = create_bitmap(2, 2);
    clear_to_color(etoile, makecol(255, 255, 255));
    for (int i = 0; i < 200; i++) {
        blit(etoile, b->fond, 0, 0, rand() % b->fond->w, rand() % b->fond->h, etoile->w, etoile->h);
    }
}

void display (Bitmaps *b, Assets *assets) {
    clear_bitmap(b->buffer);
    masked_blit(b->fond, b->buffer, 0, 0, 0, 0, b->fond->w, b->fond->h);
    masked_blit(assets->player_sprites, b->buffer, 152, 336, SCREEN_W/3+100, 425, 48, 64);
    blit(b->buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void destroy_display (Bitmaps *b, Assets *assets) {
    free_assets(assets);
    destroy_bitmap(b->buffer);
}