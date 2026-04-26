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

    if (boss->active) masked_blit(assets->boss, b->buffer, 0, 0, boss->x, boss->y, SCREEN_W, SCREEN_H);
    // fond gris
    rectfill(b->buffer, 10, 10, 210, 25, makecol(80, 80, 80));
    // barre rouge proportionnelle aux PV
    int largeur = (int)(200 * boss->pv / BOSS_MAX_PV);
    rectfill(b->buffer, 10, 10, 10 + largeur, 25, makecol(255, 0, 0));

    if (boss->pv <= 0) {
        boss->active=0;
        boss->exp_timer++;
        if (boss->exp_timer >= 5) {  // vitesse animation
            boss->exp_timer = 0;
            boss->exp_frame++;
        }

        if (boss->exp_frame < 50) {  // 50 frames au total (5 lignes x 10 colonnes)
            int col = boss->exp_frame % 10;   // colonne 0-9
            int row = boss->exp_frame / 10;   // ligne 0-4

            masked_stretch_blit(assets->explosion1, b->buffer, col*100, row*100,100,100, boss->x, boss->y,300,185);
        }
    }

    display_eclair(b, boss, boss->eclair_active, boss->active);

    blit(b->buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void destroy_display (Bitmaps *b, Assets *assets) {
    free_assets(assets);
    destroy_bitmap(b->buffer);
    destroy_bitmap(b->fond);
    destroy_bitmap(b->ship);
    destroy_bitmap(b->asteroid);
}

void display_eclair(Bitmaps *b, Boss *boss, int active, int boss_active) {
    if (!active || !boss_active) return;

    int pos_x[] = {100, 200};
    int colors[] = {makecol(50, 0, 150), makecol(120, 0, 255), makecol(255, 255, 255)};
    int sizes[] = {15, 7, 2}; // glow_éxt, corps, centre

    int segments = 20;
    int offset_y = 160;
    // Calcul pour que la fin du dernier segment soit SCREEN_H
    int seg_h = (SCREEN_H - offset_y) / segments;

    for (int i = 0; i < 2; i++) {
        int cur_x = boss->x + pos_x[i];

        for (int j = 0; j < segments; j++) {
            int next_x = (boss->x + pos_x[i]) + (rand() % 21 - 10);
            int y1 = j * seg_h + offset_y;
            // Pour le dernier segment, on force y2 à SCREEN_H
            int y2 = (j == segments - 1) ? SCREEN_H : (j + 1) * seg_h + offset_y;

            for (int k = 0; k < 3; k++) {
                for (int w = -sizes[k]; w <= sizes[k]; w++) {
                    line(b->buffer, cur_x + w, y1, next_x + w, y2, colors[k]);
                }
            }
            cur_x = next_x;
        }
    }
}