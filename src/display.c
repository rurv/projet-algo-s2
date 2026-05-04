#include "../headers/display.h"
#include "../headers/asteroids.h"

static int ground_color_for_level(int idx) {
    switch (idx) {
        case 0:  return makecol(169, 169, 169);
        case 1:  return makecol(188,  54,  34);
        case 2:  return makecol( 58,   0,  83);
        default: return makecol(169, 169, 169);
    }
}

static void draw_level_label(BITMAP *buf, const Game *game) {
    char label[32];
    if (game_is_boss(game))
        snprintf(label, sizeof(label), "Niveau BOSS");
    else
        snprintf(label, sizeof(label), "Niveau %d", game_level_index(game) + 1);

    int scale = 3;
    int tw = text_length(font, label), th = text_height(font);
    BITMAP *tmp = create_bitmap(tw, th);
    if (!tmp) return;
    clear_to_color(tmp, makecol(0, 0, 0));
    textout_ex(tmp, font, label, 0, 0, makecol(255, 255, 255), makecol(0, 0, 0));
    stretch_blit(tmp, buf, 0, 0, tw, th,
                 (SCREEN_W - tw * scale) / 2, 14, tw * scale, th * scale);
    destroy_bitmap(tmp);
}

void initialisation_allegro(void) {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) != 0) {
        allegro_message("probleme mode graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    show_mouse(screen);
}

void init_display(Bitmaps *b, Assets *assets) {
    initialisation_allegro();
    b->buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!b->buffer) { allegro_message("Erreur buffer"); allegro_exit(); exit(1); }
    load_assets(assets);

    b->fond = create_bitmap(b->buffer->w, b->buffer->h);
    clear_to_color(b->fond, makecol(255, 0, 255));
    BITMAP *star = create_bitmap(2, 2);
    clear_to_color(star, makecol(255, 255, 255));
    b->fond_scroll_x = 0.0f;
    for (int i = 0; i < 200; i++)
        blit(star, b->fond, 0, 0, rand() % b->fond->w, rand() % b->fond->h, 2, 2);
    destroy_bitmap(star);
}

void display(Bitmaps *b, Assets *assets, Player *p, Boss *boss, const Game *game) {
    clear_bitmap(b->buffer);

    // Fond parallaxe
    b->fond_scroll_x += p->dx * 0.35f;
    int ox = ((int)b->fond_scroll_x % b->fond->w + b->fond->w) % b->fond->w;
    masked_blit(b->fond, b->buffer, ox, 0, 0,           0, SCREEN_W - ox, SCREEN_H);
    masked_blit(b->fond, b->buffer,  0, 0, SCREEN_W - ox, 0, ox,          SCREEN_H);

    // Sol (absent au niveau boss)
    if (!game_is_boss(game)) {
        int gh  = game_ground_height();
        int gy  = SCREEN_H - gh;
        int col = ground_color_for_level(game_level_index(game));
        rectfill(b->buffer, 0, gy, SCREEN_W - 1, SCREEN_H - 1, col);
        int r = getr(col)+50; if(r>255)r=255;
        int g = getg(col)+50; if(g>255)g=255;
        int bv= getb(col)+50; if(bv>255)bv=255;
        hline(b->buffer, 0, gy, SCREEN_W - 1, makecol(r, g, bv));
    }

    // Astéroides (niveaux normaux uniquement)
    if (!game_is_boss(game))
        asteroids_draw(b->buffer, (AsteroidManager *)&game->am);

    // Lasers
    for (int i = 0; i < p->laser_count; i++) {
        if (!p->lasers[i].active) continue;
        masked_blit(assets->laser_sprite, b->buffer,
                    0, p->lasers[i].frame * 66,
                    p->lasers[i].x - 3, p->lasers[i].y - 50, 22, 66);
    }

    // Vaisseau joueur
    BITMAP *sub = create_sub_bitmap(assets->player_sprites, 152, 336, 48, 64);
    stretch_sprite(b->buffer, sub, p->x, p->y, 60, 84);
    destroy_bitmap(sub);

    // Boss (niveau boss uniquement)
    if (game_is_boss(game) && boss->active)
        masked_blit(assets->boss, b->buffer, 0, 0, boss->x, boss->y, SCREEN_W, SCREEN_H);

    // Explosion boss
    if (game_is_boss(game) && boss->pv <= 0) {
        boss->active = 0;
        boss->exp_timer++;
        if (boss->exp_timer >= 5) { boss->exp_timer = 0; boss->exp_frame++; }
        if (boss->exp_frame < 50) {
            int c = boss->exp_frame % 10, row = boss->exp_frame / 10;
            masked_stretch_blit(assets->explosion1, b->buffer,
                                c*100, row*100, 100, 100, boss->x, boss->y, 300, 185);
        }
    }

    // Éclairs boss
    if (game_is_boss(game))
        display_eclair(b, boss, boss->eclair_active, boss->active);

    // Image claude
    //masked_blit(assets->claude, b->buffer, 0, 0,SCREEN_W/2 - 160, SCREEN_H/2 - 146, 320, 292);

    // HUD
    display_hud(b, p, boss, game);

    // Libellé niveau
    draw_level_label(b->buffer, game);

    blit(b->buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void display_eclair(Bitmaps *b, Boss *boss, int active, int boss_active) {
    if (!active || !boss_active) return;
    int pos_x[]  = {100, 200};
    int colors[] = {makecol(50,0,150), makecol(120,0,255), makecol(255,255,255)};
    int sizes[]  = {15, 7, 2};
    int segments = 20, offset_y = 160;
    int seg_h = (SCREEN_H - offset_y) / segments;
    for (int i = 0; i < 2; i++) {
        int cur_x = boss->x + pos_x[i];
        for (int j = 0; j < segments; j++) {
            int next_x = (boss->x + pos_x[i]) + (rand() % 21 - 10);
            int y1 = j * seg_h + offset_y;
            int y2 = (j == segments-1) ? SCREEN_H : (j+1)*seg_h + offset_y;
            for (int k = 0; k < 3; k++)
                for (int w = -sizes[k]; w <= sizes[k]; w++)
                    line(b->buffer, cur_x+w, y1, next_x+w, y2, colors[k]);
            cur_x = next_x;
        }
    }
}

void display_hud(Bitmaps *b, Player *p, Boss *boss, const Game *game) {
    // Barre de vie boss (uniquement au niveau boss)
    if (game_is_boss(game)) {
        int bx=10, by=10, bw=200, bh=18;
        rectfill(b->buffer, bx-2, by-2, bx+bw+2, by+bh+2, makecol(20,0,0));
        rectfill(b->buffer, bx, by, bx+bw, by+bh, makecol(60,0,0));
        int larg = (int)(bw * boss->pv / BOSS_MAX_PV);
        if (larg > 0) {
            rectfill(b->buffer, bx, by,       bx+larg, by+bh,   makecol(180,0,0));
            rectfill(b->buffer, bx, by,       bx+larg, by+bh/2, makecol(255,40,40));
            rectfill(b->buffer, bx, by+bh-3,  bx+larg, by+bh,   makecol(100,0,0));
        }
        rect(b->buffer, bx-1, by-1, bx+bw+1, by+bh+1, makecol(255,80,80));
        textout_ex(b->buffer, font, "BOSS", bx+bw+8, by+3, makecol(255,80,80), -1);
    } else {
        // Compteur d'astéroïdes restants
        char buf[32];
        AsteroidManager *am = (AsteroidManager *)&game->am;
        int remaining = am->count + (am->to_spawn - am->spawned);
        snprintf(buf, sizeof(buf), "Asteroides: %d", remaining);
        textout_ex(b->buffer, font, buf, 10, 10, makecol(255,255,255), -1);
    }

    // Cœurs joueur
    for (int v = 0; v < 3; v++) {
        int hx = SCREEN_W - 20 - v*28, hy = 10;
        int col  = (v < p->vies) ? makecol(255,60,100)  : makecol(80,80,80);
        int col2 = (v < p->vies) ? makecol(255,150,170) : makecol(60,60,60);
        putpixel(b->buffer,hx+2,hy+0,col); putpixel(b->buffer,hx+3,hy+0,col);
        putpixel(b->buffer,hx+7,hy+0,col); putpixel(b->buffer,hx+8,hy+0,col);
        for(int i=1;i<=9; i++) putpixel(b->buffer,hx+i,hy+1,col);
        for(int i=0;i<=10;i++) putpixel(b->buffer,hx+i,hy+2,col);
        putpixel(b->buffer,hx+1,hy+2,col2); putpixel(b->buffer,hx+2,hy+2,col2);
        for(int i=0;i<=10;i++) putpixel(b->buffer,hx+i,hy+3,col);
        for(int i=0;i<=10;i++) putpixel(b->buffer,hx+i,hy+4,col);
        putpixel(b->buffer,hx+1,hy+3,col2);
        for(int i=1;i<=9; i++) putpixel(b->buffer,hx+i,hy+5,col);
        for(int i=2;i<=8; i++) putpixel(b->buffer,hx+i,hy+6,col);
        for(int i=3;i<=7; i++) putpixel(b->buffer,hx+i,hy+7,col);
        for(int i=4;i<=6; i++) putpixel(b->buffer,hx+i,hy+8,col);
        putpixel(b->buffer,hx+5,hy+9,col);
    }
}

void destroy_display(Bitmaps *b, Assets *assets) {
    free_assets(assets);
    destroy_bitmap(b->buffer);
    destroy_bitmap(b->fond);
    destroy_bitmap(b->ship);
    destroy_bitmap(b->asteroid);
}
