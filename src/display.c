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
    display_hud(b, p, boss);

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
    masked_blit(assets->claude, b->buffer, 0, 0, SCREEN_W/2 - 160, SCREEN_H/2 - 146, 320, 292);
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

void display_hud(Bitmaps *b, Player *p, Boss *boss) {
    // === BARRE DE VIE BOSS STYLÉE ===
    int bar_x = 10, bar_y = 10, bar_w = 200, bar_h = 18;

    rectfill(b->buffer, bar_x - 2, bar_y - 2, bar_x + bar_w + 2, bar_y + bar_h + 2, makecol(20, 0, 0));
    rectfill(b->buffer, bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, makecol(60, 0, 0));

    int largeur = (int)(bar_w * boss->pv / BOSS_MAX_PV);
    if (largeur > 0) {
        rectfill(b->buffer, bar_x, bar_y,           bar_x + largeur, bar_y + bar_h,     makecol(180, 0, 0));
        rectfill(b->buffer, bar_x, bar_y,           bar_x + largeur, bar_y + bar_h/2,   makecol(255, 40, 40));
        rectfill(b->buffer, bar_x, bar_y + bar_h-3, bar_x + largeur, bar_y + bar_h,     makecol(100, 0, 0));
    }

    rect(b->buffer, bar_x - 1, bar_y - 1, bar_x + bar_w + 1, bar_y + bar_h + 1, makecol(255, 80, 80));
    textout_ex(b->buffer, font, "BOSS", bar_x + bar_w + 8, bar_y + 3, makecol(255, 80, 80), -1);

    // === CŒURS DU JOUEUR ===
    for (int v = 0; v < 3; v++) {
        int hx = SCREEN_W - 20 - v * 28;
        int hy = 10;
        int col  = (v < p->vies) ? makecol(255, 60, 100) : makecol(80, 80, 80);
        int col2 = (v < p->vies) ? makecol(255, 150, 170) : makecol(60, 60, 60);

        putpixel(b->buffer, hx+2, hy+0, col); putpixel(b->buffer, hx+3, hy+0, col);
        putpixel(b->buffer, hx+7, hy+0, col); putpixel(b->buffer, hx+8, hy+0, col);
        for (int i = 1; i <= 9;  i++) putpixel(b->buffer, hx+i, hy+1, col);
        for (int i = 0; i <= 10; i++) putpixel(b->buffer, hx+i, hy+2, col);
        putpixel(b->buffer, hx+1, hy+2, col2); putpixel(b->buffer, hx+2, hy+2, col2);
        for (int i = 0; i <= 10; i++) putpixel(b->buffer, hx+i, hy+3, col);
        for (int i = 0; i <= 10; i++) putpixel(b->buffer, hx+i, hy+4, col);
        putpixel(b->buffer, hx+1, hy+3, col2);
        for (int i = 1; i <= 9;  i++) putpixel(b->buffer, hx+i, hy+5, col);
        for (int i = 2; i <= 8;  i++) putpixel(b->buffer, hx+i, hy+6, col);
        for (int i = 3; i <= 7;  i++) putpixel(b->buffer, hx+i, hy+7, col);
        for (int i = 4; i <= 6;  i++) putpixel(b->buffer, hx+i, hy+8, col);
        putpixel(b->buffer, hx+5, hy+9, col);
    }
}