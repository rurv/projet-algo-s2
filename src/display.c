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

void display (BITMAP *buffer, Assets *assets) {
    clear_bitmap(buffer);
    masked_blit(assets->player_sprites, buffer, 152, 336, SCREEN_WIDTH/3+180, 650, 48, 64);
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void destroy_display (BITMAP **buffer, Assets *assets) {
    free_assets(assets);
    destroy_bitmap(*buffer);
}

void update_display (BITMAP *buffer, Assets *assets, Player *player) {
    clear_bitmap(buffer);

    int sx = 152; // Coordonnée X dans .bmp
    int sy = 336; // Coordonnée Y dans .bmp
    int sw = 48;  // Largeur du sprite
    int sh = 64;  // Hauteur du sprite

    player->x += player->dx;

    if (player->x < 0) player->x = 0;
    if (player->x > SCREEN_W - sw) player->x = SCREEN_W - sw;

    masked_blit(assets->player_sprites, buffer, sx, sy, player->x, 650, sw, sh);
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}