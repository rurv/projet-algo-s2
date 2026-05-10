#include "../headers/display.h"
#include "../headers/asteroids.h"
#include <stdio.h>

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
        int idx = game_level_index(game);
        if (idx >= 3) idx = 2;
        // Étire le BMP sur toute la largeur et la hauteur du sol
        stretch_blit(assets->sol_sprites[idx], b->buffer,
                     0, 0,
                     assets->sol_sprites[idx]->w, assets->sol_sprites[idx]->h,
                     0, gy, SCREEN_W, gh);
    }

    // Astéroides (niveaux normaux uniquement)
    // Le sprite est choisi en fonction du niveau (1->idx 0, 2->idx 1, 3->idx 2)
    if (!game_is_boss(game)) {
        int idx = game_level_index(game);
        // Sécurité : si plus de niveaux que de sprites, on réutilise le dernier
        if (idx >= NB_ASTEROID_SPRITES) idx = NB_ASTEROID_SPRITES - 1;
        asteroids_draw(assets->asteroid_sprites[idx], b->buffer,
                       (AsteroidManager *)&game->am);
    }
    // Lasers
    for (int i = 0; i < p->laser_count; i++) {
        if (!p->lasers[i].active) continue;
        masked_blit(assets->laser_sprite, b->buffer,
                    0, p->lasers[i].frame * 66,
                    p->lasers[i].x - 3, p->lasers[i].y - 50, 22, 66);
    }

    // Vaisseau joueur
    int skin_x = 0, skin_y = 0, skin_w = 0, skin_h = 0;

    switch(p->skin_id) {
        case VAISSEAU1: skin_x = 152; skin_y = 336; skin_w = 48; skin_h = 64; break;
        case VAISSEAU2: skin_x = 208; skin_y = 328; skin_w = 47; skin_h = 63; break;
        case VAISSEAU3: skin_x = 64; skin_y = 368; skin_w = 47; skin_h = 40;break;
        case VAISSEAU4: skin_x = 136; skin_y = 416; skin_w = 47; skin_h = 31;break;
    }

    // --- DESSIN DU VAISSEAU ---
    int afficher = 1;

    if (p->invincible) {
        // CONDITION CRUCIALE : On ne clignote que si le timer de dégât est actif
        // Si c'est un bonus (invincible = 1 mais timer = 0), on n'entre pas ici
        if (p->invincible_timer > 0) {
            if (p->invincible_timer % 10 < 5) {
                afficher = 0;
            }
        }
    }

    if (afficher) {
        // Ton code stretch_sprite habituel ici
        BITMAP *sub = create_sub_bitmap(assets->player_sprites, skin_x, skin_y, skin_w, skin_h);
        stretch_sprite(b->buffer, sub, p->x, p->y, 60, 84);
        destroy_bitmap(sub);
    }

    if (p->invincible)  draw_neon_circle(b->buffer, p->x, p->y, 45);


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

    draw_bonus_system(b->buffer, (Game *)game, assets);

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

void draw_bonus_system(BITMAP *buffer, Game *g, Assets *as) {
    // 1. Dessiner la boule Neon si active
    if (g->bonus.actif) {
        int col = (g->bonus.type == INVINCIBILITE) ? makecol(255, 255, 0) : makecol(0, 255, 255);
        for (int r = 1; r < 12; r++) { // Effet Glow
            circle(buffer, g->bonus.x, g->bonus.y, 8 + r, col);
        }
        circlefill(buffer, g->bonus.x, g->bonus.y, 8, makecol(255, 255, 255));
    }

    // 2. Barre de HUD pour le temps restant
    if (g->bonus_timer > 0) {
        // 1. On définit la durée max selon le bonus pour que la barre soit toujours pleine au début
        int duree_max = (g->bonus_actif == INVINCIBILITE) ? 600 : 200;

        // 2. Calcul de la largeur (ici 150 pixels de large pour que ça tienne bien à droite)
        int largeur_max_barre = 150;
        int largeur_actuelle = (g->bonus_timer * largeur_max_barre) / duree_max;

        // 3. Choix de la couleur
        int col_barre = (g->bonus_actif == INVINCIBILITE) ? makecol(255, 215, 0) : makecol(0, 191, 255);

        // 4. Coordonnées en haut à droite (sous les cœurs)
        // On se base sur SCREEN_W - marge
        int posX = SCREEN_W - 170; // Position de début de la barre
        int posY = 45;             // Juste en dessous des cœurs (qui sont souvent à y=10 ou 20)

        // Dessin du contour (en blanc)
        rect(buffer, posX - 1, posY - 1, posX + largeur_max_barre + 1, posY + 11, makecol(255, 255, 255));

        // Dessin de la barre de remplissage
        rectfill(buffer, posX, posY, posX + largeur_actuelle, posY + 10, col_barre);

        // Petit texte descriptif à gauche de la barre ou au-dessus
        textout_right_ex(buffer, font, "BONUS:", posX - 10, posY + 2, makecol(255, 255, 255), -1);
    }
}

void destroy_display(Bitmaps *b, Assets *assets) {
    free_assets(assets);
    destroy_bitmap(b->buffer);
    destroy_bitmap(b->fond);
    destroy_bitmap(b->ship);
    destroy_bitmap(b->asteroid);
}

void draw_neon_circle(BITMAP *dest, int x, int y, int radius) {
    // On centre sur le vaisseau (60x84)
    int cx = x + 30;
    int cy = y + 42;

    // 1. Le cœur du néon (très brillant, presque blanc)
    circle(dest, cx, cy, radius,     makecol(255, 255, 200));
    circle(dest, cx, cy, radius - 1, makecol(255, 255, 150));

    // 2. Le "Glow" (halos extérieurs)
    // On dessine des cercles de plus en plus grands et de plus en plus sombres
    circle(dest, cx, cy, radius + 1, makecol(220, 220, 0));
    circle(dest, cx, cy, radius + 2, makecol(180, 180, 0));
    circle(dest, cx, cy, radius + 3, makecol(130, 130, 0));
    circle(dest, cx, cy, radius + 4, makecol(80, 80, 0));

    // 3. Un petit éclat interne
    circle(dest, cx, cy, radius - 2, makecol(200, 200, 0));
}

// ── Helpers privés ────────────────────────────────────────────────────────────

static void sync_fond_with_etoiles(BITMAP *fond, Etoile *etoiles, int n) {
    clear_to_color(fond, makecol(255, 0, 255));
    for (int i = 0; i < n; i++) {
        int gris = etoiles[i].luminosite;
        if (etoiles[i].x + 1 < fond->w && etoiles[i].y + 1 < fond->h) {
            putpixel(fond, etoiles[i].x,     etoiles[i].y,     makecol(gris, gris, gris));
            putpixel(fond, etoiles[i].x + 1, etoiles[i].y,     makecol(gris, gris, gris));
            putpixel(fond, etoiles[i].x,     etoiles[i].y + 1, makecol(gris, gris, gris));
            putpixel(fond, etoiles[i].x + 1, etoiles[i].y + 1, makecol(gris, gris, gris));
        }
    }
}

static void draw_player_ship(BITMAP *dest, Player *p, Assets *assets) {
    int skin_x = 0, skin_y = 0, skin_w = 0, skin_h = 0;
    switch (p->skin_id) {
        case VAISSEAU1: skin_x=152; skin_y=336; skin_w=48; skin_h=64; break;
        case VAISSEAU2: skin_x=208; skin_y=328; skin_w=47; skin_h=63; break;
        case VAISSEAU3: skin_x= 64; skin_y=368; skin_w=47; skin_h=40; break;
        case VAISSEAU4: skin_x=136; skin_y=416; skin_w=47; skin_h=31; break;
    }
    BITMAP *sub = create_sub_bitmap(assets->player_sprites, skin_x, skin_y, skin_w, skin_h);
    if (sub) {
        stretch_sprite(dest, sub, (int)p->x, (int)p->y, 60, 84);
        destroy_bitmap(sub);
    }
}

// ── API publique de la cinématique ────────────────────────────────────────────

void transition_reset(Transition *tr) {
    tr->phase        = TRANS_INACTIVE;
    tr->timer        = 0;
    tr->star_speed   = 1.0f;
    tr->sol_offset   = 0.0f;
    tr->ship_anim_y  = 0.0f;
    tr->level_changed = 0;
}

void transition_start(Transition *tr) {
    tr->phase        = TRANS_CENTER;
    tr->timer        = 0;
    tr->star_speed   = 1.0f;
    tr->sol_offset   = 0.0f;
    tr->level_changed = 0;
}

// Met à jour les étoiles selon la vitesse de la cinématique.
// À appeler chaque frame quand la transition est active.
void transition_update_etoiles(Transition *tr, Etoile *etoiles, int n_etoiles) {
    for (int i = 0; i < n_etoiles; i++) {
        int dy = (tr->phase != TRANS_INACTIVE)
                 ? (int)(etoiles[i].vitesse * tr->star_speed)
                 : etoiles[i].vitesse;
        etoiles[i].y += dy;
        if (etoiles[i].y >= SCREEN_HEIGHT) {
            etoiles[i].y = 0;
            etoiles[i].x = rand() % SCREEN_WIDTH;
        }
    }
}

// Avance la machine à états de la cinématique d'une frame.
// Retourne 1 tant que la cinématique est active, 0 quand elle est terminée.
int transition_update(Transition *tr, Player *p, Game *game, Boss *boss,
                      int *boss_dead_sound, float player_game_y) {
    if (tr->phase == TRANS_INACTIVE) return 0;

    if (tr->phase == TRANS_CENTER) {
        tr->timer++;
        float t    = (float)tr->timer / TRANS_CENTER_DUR;
        if (t > 1.0f) t = 1.0f;
        float ease = 1.0f - (1.0f - t) * (1.0f - t);
        float target_x = (float)(SCREEN_W / 2 - 30);
        p->x  = p->x + (target_x - p->x) * ease;
        p->dx = 0.0f;

        if (tr->timer >= TRANS_CENTER_DUR) {
            p->x      = target_x;
            tr->phase = TRANS_ACCEL;
            tr->timer = 0;
        }
    }
    else if (tr->phase == TRANS_ACCEL) {
        tr->timer++;
        float t        = (float)tr->timer / TRANS_ACCEL_DUR;
        tr->star_speed = 1.0f + t * t * 29.0f;
        tr->sol_offset += tr->star_speed * 2.0f;

        if (tr->timer >= TRANS_ACCEL_DUR) {
            tr->phase = TRANS_WARP;
            tr->timer = 0;
        }
    }
    else if (tr->phase == TRANS_WARP) {
        tr->timer++;
        tr->star_speed = 30.0f;
        p->y -= 25.0f;

        if (!tr->level_changed && p->y < -84.0f) {
            game_next_level(game, boss);
            game->level_complete = 0;
            *boss_dead_sound     = 0;
            tr->level_changed    = 1;
            p->x  = (float)(SCREEN_W / 2 - 30);
            p->y  = -200.0f;
            p->dx = 0.0f;
            p->dy = 0.0f;
        }

        if (tr->level_changed && tr->timer >= TRANS_WARP_DUR) {
            tr->phase = TRANS_ARRIVE;
            tr->timer = 0;
        }
    }
    else if (tr->phase == TRANS_ARRIVE) {
        tr->timer++;
        float t        = (float)tr->timer / TRANS_ARRIVE_DUR;
        tr->star_speed = 30.0f * (1.0f - t) + 1.0f;
        float ease_in  = t * t;
        tr->sol_offset = (float)SCREEN_H * (1.0f - ease_in);

        if (tr->timer >= TRANS_ARRIVE_DUR) {
            tr->sol_offset = 0.0f;
            tr->star_speed = 1.0f;
            tr->phase      = TRANS_DESCEND;
            tr->timer      = 0;
            p->y           = -100.0f;
        }
    }
    else if (tr->phase == TRANS_DESCEND) {
        tr->timer++;
        float t    = (float)tr->timer / TRANS_DESCEND_DUR;
        if (t > 1.0f) t = 1.0f;
        float ease = 1.0f - (1.0f - t) * (1.0f - t);
        p->y  = -100.0f + (player_game_y - (-100.0f)) * ease;
        p->dx = 0.0f;
        p->dy = 0.0f;

        if (tr->timer >= TRANS_DESCEND_DUR) {
            // Fin : on ne peut pas accéder à bmps ici,
            // la synchronisation du fond se fait dans display_transition()
            p->y           = player_game_y;
            p->x           = (float)(SCREEN_W / 2 - 30);
            tr->phase      = TRANS_INACTIVE;
            tr->star_speed = 1.0f;
            tr->sol_offset = 0.0f;
            tr->level_changed = 0;
            return 0; // transition terminée cette frame
        }
    }

    return 1; // toujours active
}

// Dessine la frame de cinématique (fond étoilé + sol avec offset + vaisseau).
// Appeler après transition_update() si celui-ci retourne 1,
// ET aussi la dernière frame où il retourne 0 (pour la sync du fond).
void display_transition(Transition *tr, Bitmaps *b, Assets *assets,
                        Player *p, Game *game, Etoile *etoiles, int n_etoiles) {
    clear_bitmap(b->buffer);

    // Fond étoilé depuis le tableau
    for (int i = 0; i < n_etoiles; i++) {
        int gris = etoiles[i].luminosite;
        putpixel(b->buffer, etoiles[i].x, etoiles[i].y, makecol(gris, gris, gris));
    }

    // Sol avec offset (niveaux non-boss uniquement)
    if (!game_is_boss(game)) {
        int gh  = game_ground_height();
        int gy  = SCREEN_H - gh + (int)tr->sol_offset;
        int idx = game_level_index(game);
        if (idx >= 3) idx = 2;
        if (gy < SCREEN_H) {
            stretch_blit(assets->sol_sprites[idx], b->buffer,
                         0, 0,
                         assets->sol_sprites[idx]->w,
                         assets->sol_sprites[idx]->h,
                         0, gy, SCREEN_W, gh);
        }
    }

    // Vaisseau : visible seulement dans le champ
    if (p->y > -84.0f && p->y < (float)SCREEN_H)
        draw_player_ship(b->buffer, p, assets);

    // Si la transition vient juste de se terminer (phase repassée à INACTIVE),
    // on synchronise bmps.fond pour que le rendu normal reprenne sans saut.
    if (tr->phase == TRANS_INACTIVE) {
        sync_fond_with_etoiles(b->fond, etoiles, n_etoiles);
        b->fond_scroll_x = 0.0f;
    }

    blit(b->buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}