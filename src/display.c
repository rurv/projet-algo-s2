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
    b->fond_scroll_x = 0.0;
    for (int i = 0; i < 200; i++) {
        blit(etoile, b->fond, 0, 0, rand() % b->fond->w, rand() % b->fond->h, etoile->w, etoile->h);
    }
    destroy_bitmap(etoile);
}

void display (Bitmaps *b, Assets *assets, Player *p, Boss *boss) {
    clear_bitmap(b->buffer);
    /*if (key[KEY_E]) {
        int x = SCREEN_W / 2;
        int segments = 8;
        int seg_h = SCREEN_H / segments;
        int x_courant = x;

        for (int i = 0; i < segments; i++) {
            int x_next = x + (rand() % 20 - 10);
            int y_debut = i * seg_h;
            int y_fin   = (i + 1) * seg_h;

            line(b->buffer, x_courant - 2, y_debut, x_next + 2, y_fin, makecol(80, 0, 120));
            line(b->buffer, x_courant - 1, y_debut, x_next + 1, y_fin, makecol(150, 0, 255));
            line(b->buffer, x_courant,     y_debut, x_next,     y_fin, makecol(255, 255, 255));
            line(b->buffer, x_courant + 1, y_debut, x_next - 1, y_fin, makecol(150, 0, 255));
            line(b->buffer, x_courant + 2, y_debut, x_next - 2, y_fin, makecol(80, 0, 120));

            x_courant = x_next;
        }
    }*/
    b->fond_scroll_x += p->dx * 0.35;

    int ox = ((int)b->fond_scroll_x % b->fond->w + b->fond->w) % b->fond->w;

    masked_blit(b->fond, b->buffer, ox, 0, 0, 0, SCREEN_W - ox, SCREEN_H);
    masked_blit(b->fond, b->buffer, 0, 0, SCREEN_W - ox, 0, ox, SCREEN_H);

    for (int i = 0; i < p->laser_count; i++) {
        if (p->lasers[i].active) {
            masked_blit(assets->laser_sprite, b->buffer,
                        0, p->lasers[i].frame * 66,
                        p->lasers[i].x-3,
                        p->lasers[i].y-50,
                        22, 66);
        }
    }

    BITMAP *sub = create_sub_bitmap(assets->player_sprites, 152, 336, 48, 64);
    stretch_sprite(b->buffer, sub, p->x, p->y, 60, 84);
    destroy_bitmap(sub);
    masked_blit(assets->boss, b->buffer, 0, 0, boss->x, boss->y, SCREEN_W, SCREEN_H);
    // fond gris
    rectfill(b->buffer, 10, 10, 210, 25, makecol(80, 80, 80));
    // barre rouge proportionnelle aux PV
    int largeur = (int)(200 * boss->pv / BOSS_MAX_PV);
    rectfill(b->buffer, 10, 10, 10 + largeur, 25, makecol(255, 0, 0));
    blit(b->buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void destroy_display (Bitmaps *b, Assets *assets) {
    free_assets(assets);
    destroy_bitmap(b->buffer);
    destroy_bitmap(b->fond);
    destroy_bitmap(b->ship);
    destroy_bitmap(b->asteroid);
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