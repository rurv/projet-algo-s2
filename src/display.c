#include "../headers/display.h"
#include "../headers/asteroids.h"
#include "../headers/ihm.h"
#include <stdio.h>
#include <math.h>

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

// Dessin d'un vaisseau joueur (réutilisé dans plusieurs contextes)
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
    masked_blit(b->fond, b->buffer, ox, 0, 0,            0, SCREEN_W - ox, SCREEN_H);
    masked_blit(b->fond, b->buffer,  0, 0, SCREEN_W - ox, 0, ox,           SCREEN_H);

    // Sol (absent au niveau boss)
    if (!game_is_boss(game)) {
        int gh  = game_ground_height();
        int gy  = SCREEN_H - gh;
        int idx = game_level_index(game);
        if (idx >= 3) idx = 2;
        stretch_blit(assets->sol_sprites[idx], b->buffer,
                     0, 0,
                     assets->sol_sprites[idx]->w, assets->sol_sprites[idx]->h,
                     0, gy, SCREEN_W, gh);
    }

    // Astéroïdes (niveaux normaux uniquement)
    if (!game_is_boss(game)) {
        int idx = game_level_index(game);
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
    switch (p->skin_id) {
        case VAISSEAU1: skin_x=152; skin_y=336; skin_w=48; skin_h=64; break;
        case VAISSEAU2: skin_x=208; skin_y=328; skin_w=47; skin_h=63; break;
        case VAISSEAU3: skin_x= 64; skin_y=368; skin_w=47; skin_h=40; break;
        case VAISSEAU4: skin_x=136; skin_y=416; skin_w=47; skin_h=31; break;
    }

    int afficher = 1;
    if (p->invincible && p->invincible_timer > 0) {
        if (p->invincible_timer % 10 < 5) afficher = 0;
    }
    if (afficher) {
        BITMAP *sub = create_sub_bitmap(assets->player_sprites, skin_x, skin_y, skin_w, skin_h);
        stretch_sprite(b->buffer, sub, p->x, p->y, 60, 84);
        destroy_bitmap(sub);
    }
    if (p->invincible) draw_neon_circle(b->buffer, p->x, p->y, 45);

    // Boss avec tremblement éventuel pendant la séquence de mort
    if (game_is_boss(game) && boss->active) {
        int draw_x = (int)(boss->x + boss->shake_x);
        masked_blit(assets->boss, b->buffer, 0, 0, draw_x, boss->y, SCREEN_W, SCREEN_H);

        // Mini-explosions pendant le tremblement
        if (boss->dying && boss->exp_mini_active) {
            int c   = boss->exp_mini_frame % 10;
            int row = boss->exp_mini_frame / 10;
            int ex  = (int)boss->exp_mini_x;
            int ey  = (int)boss->exp_mini_y;
            // Explosion réduite (60x40 au lieu de 300x185)
            masked_stretch_blit(assets->explosion1, b->buffer,
                                c * 100, row * 100, 100, 100,
                                ex, ey, 60, 40);
        }
    }

    // Grande explosion finale du boss
    if (game_is_boss(game) && boss->pv <= 0 && !boss->dying && boss->death_done) {
        boss->active = 0;
        boss->exp_timer++;
        if (boss->exp_timer >= 5) { boss->exp_timer = 0; boss->exp_frame++; }
        if (boss->exp_frame < 50) {
            int c = boss->exp_frame % 10, row = boss->exp_frame / 10;
            masked_stretch_blit(assets->explosion1, b->buffer,
                                c * 100, row * 100, 100, 100,
                                boss->x, boss->y, 300, 185);
        }
    }

    // Éclairs du boss
    if (game_is_boss(game))
        display_eclair(b, boss, boss->eclair_active, boss->active);

    display_hud(b, p, boss, game);
    draw_level_label(b->buffer, game);
    draw_bonus_system(b->buffer, (Game *)game, assets);

    blit(b->buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void display_eclair(Bitmaps *b, Boss *boss, int active, int boss_active) {
    if (!active || !boss_active) return;
    int pos_x[]  = { 100, 200 };
    int colors[] = { makecol(50,0,150), makecol(120,0,255), makecol(255,255,255) };
    int sizes[]  = { 15, 7, 2 };
    int segments = 20, offset_y = 160;
    int seg_h = (SCREEN_H - offset_y) / segments;
    for (int i = 0; i < 2; i++) {
        int cur_x = boss->x + pos_x[i];
        for (int j = 0; j < segments; j++) {
            int next_x = (boss->x + pos_x[i]) + (rand() % 21 - 10);
            int y1 = j * seg_h + offset_y;
            int y2 = (j == segments - 1) ? SCREEN_H : (j + 1) * seg_h + offset_y;
            for (int k = 0; k < 3; k++)
                for (int w = -sizes[k]; w <= sizes[k]; w++)
                    line(b->buffer, cur_x + w, y1, next_x + w, y2, colors[k]);
            cur_x = next_x;
        }
    }
}

// Barre de PV du boss : imposante en haut de l'écran, pleine largeur avec marges
void display_hud(Bitmaps *b, Player *p, Boss *boss, const Game *game) {
    if (game_is_boss(game)) {
        // Barre de PV large en haut de l'écran
        int margin = 20;
        int bx = margin, by = 8;
        int bw = SCREEN_W - 2 * margin;
        int bh = 28;

        // Fond sombre
        rectfill(b->buffer, bx - 3, by - 3, bx + bw + 3, by + bh + 3, makecol(10, 0, 0));
        rectfill(b->buffer, bx, by, bx + bw, by + bh, makecol(50, 0, 0));

        // Remplissage proportionnel (modulaire : fonctionne pour n'importe quel pv_max)
        int larg = (boss->pv_max > 0) ? (int)((float)bw * boss->pv / boss->pv_max) : 0;
        if (larg < 0) larg = 0;
        if (larg > 0) {
            rectfill(b->buffer, bx, by,          bx + larg, by + bh,       makecol(180, 0, 0));
            // Reflet supérieur
            rectfill(b->buffer, bx, by,          bx + larg, by + bh / 3,   makecol(255, 60, 60));
            // Ombre inférieure
            rectfill(b->buffer, bx, by + bh - 4, bx + larg, by + bh,       makecol(100, 0, 0));
        }
        // Bordure lumineuse
        rect(b->buffer, bx - 2, by - 2, bx + bw + 2, by + bh + 2, makecol(255, 80, 80));

        // Texte "BOSS" centré dans la barre
        char pv_txt[32];
        snprintf(pv_txt, sizeof(pv_txt), "BOSS  %d / %d", (int)boss->pv, (int)boss->pv_max);
        int tw = text_length(font, pv_txt);
        textout_ex(b->buffer, font, pv_txt,
                   bx + (bw - tw) / 2, by + (bh - text_height(font)) / 2,
                   makecol(255, 255, 255), -1);
    } else {
        // Compteur d'astéroïdes restants
        char buf[32];
        AsteroidManager *am = (AsteroidManager *)&game->am;
        int remaining = am->count + (am->to_spawn - am->spawned);
        snprintf(buf, sizeof(buf), "Asteroides: %d", remaining);
        textout_ex(b->buffer, font, buf, 10, 10, makecol(255, 255, 255), -1);
    }

    // Cœurs joueur
    for (int v = 0; v < 3; v++) {
        int hx = SCREEN_W - 20 - v * 28, hy = 48;
        int col  = (v < p->vies) ? makecol(255, 60, 100)  : makecol(80, 80, 80);
        int col2 = (v < p->vies) ? makecol(255, 150, 170) : makecol(60, 60, 60);
        putpixel(b->buffer, hx+2, hy+0, col); putpixel(b->buffer, hx+3, hy+0, col);
        putpixel(b->buffer, hx+7, hy+0, col); putpixel(b->buffer, hx+8, hy+0, col);
        for (int i = 1; i <= 9; i++)  putpixel(b->buffer, hx+i, hy+1, col);
        for (int i = 0; i <= 10; i++) putpixel(b->buffer, hx+i, hy+2, col);
        putpixel(b->buffer, hx+1, hy+2, col2); putpixel(b->buffer, hx+2, hy+2, col2);
        for (int i = 0; i <= 10; i++) putpixel(b->buffer, hx+i, hy+3, col);
        for (int i = 0; i <= 10; i++) putpixel(b->buffer, hx+i, hy+4, col);
        putpixel(b->buffer, hx+1, hy+3, col2);
        for (int i = 1; i <= 9; i++)  putpixel(b->buffer, hx+i, hy+5, col);
        for (int i = 2; i <= 8; i++)  putpixel(b->buffer, hx+i, hy+6, col);
        for (int i = 3; i <= 7; i++)  putpixel(b->buffer, hx+i, hy+7, col);
        for (int i = 4; i <= 6; i++)  putpixel(b->buffer, hx+i, hy+8, col);
        putpixel(b->buffer, hx+5, hy+9, col);
    }
}

void draw_bonus_system(BITMAP *buffer, Game *g, Assets *as) {
    if (g->bonus.actif) {
        int col = (g->bonus.type == INVINCIBILITE) ? makecol(255, 255, 0) : makecol(0, 255, 255);
        for (int r = 1; r < 12; r++)
            circle(buffer, g->bonus.x, g->bonus.y, 8 + r, col);
        circlefill(buffer, g->bonus.x, g->bonus.y, 8, makecol(255, 255, 255));
    }

    if (g->bonus_timer > 0) {
        int duree_max      = (g->bonus_actif == INVINCIBILITE) ? 600 : 200;
        int largeur_max    = 150;
        int largeur_actuelle = (g->bonus_timer * largeur_max) / duree_max;
        int col_barre      = (g->bonus_actif == INVINCIBILITE) ? makecol(255, 215, 0) : makecol(0, 191, 255);
        int posX = SCREEN_W - 170;
        int posY = 80;
        rect(buffer, posX - 1, posY - 1, posX + largeur_max + 1, posY + 11, makecol(255, 255, 255));
        rectfill(buffer, posX, posY, posX + largeur_actuelle, posY + 10, col_barre);
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
    int cx = x + 30, cy = y + 42;
    circle(dest, cx, cy, radius,     makecol(255, 255, 200));
    circle(dest, cx, cy, radius - 1, makecol(255, 255, 150));
    circle(dest, cx, cy, radius + 1, makecol(220, 220, 0));
    circle(dest, cx, cy, radius + 2, makecol(180, 180, 0));
    circle(dest, cx, cy, radius + 3, makecol(130, 130, 0));
    circle(dest, cx, cy, radius + 4, makecol(80,  80,  0));
    circle(dest, cx, cy, radius - 2, makecol(200, 200, 0));
}

// ── Cinématique inter-niveaux ────────────────────────────────────────────────

void transition_reset(Transition *tr) {
    tr->phase         = TRANS_INACTIVE;
    tr->timer         = 0;
    tr->star_speed    = 1.0f;
    tr->sol_offset    = 0.0f;
    tr->ship_anim_y   = 0.0f;
    tr->level_changed = 0;
}

void transition_start(Transition *tr) {
    tr->phase         = TRANS_CENTER;
    tr->timer         = 0;
    tr->star_speed    = 1.0f;
    tr->sol_offset    = 0.0f;
    tr->level_changed = 0;
}

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

int transition_update(Transition *tr, Player *p, Game *game, Boss *boss,
                      int *boss_dead_sound, float player_game_y) {
    if (tr->phase == TRANS_INACTIVE) return 0;

    if (tr->phase == TRANS_CENTER) {
        tr->timer++;
        float t    = (float)tr->timer / TRANS_CENTER_DUR;
        if (t > 1.0f) t = 1.0f;
        float ease     = 1.0f - (1.0f - t) * (1.0f - t);
        float target_x = (float)(SCREEN_W / 2 - 30);
        p->x  = p->x + (target_x - p->x) * ease;
        p->dx = 0.0f;
        if (tr->timer >= TRANS_CENTER_DUR) {
            p->x      = target_x;
            tr->phase = TRANS_ACCEL;
            tr->timer = 0;
        }
    } else if (tr->phase == TRANS_ACCEL) {
        tr->timer++;
        float t        = (float)tr->timer / TRANS_ACCEL_DUR;
        tr->star_speed = 1.0f + t * t * 29.0f;
        tr->sol_offset += tr->star_speed * 2.0f;
        if (tr->timer >= TRANS_ACCEL_DUR) {
            tr->phase = TRANS_WARP;
            tr->timer = 0;
        }
    } else if (tr->phase == TRANS_WARP) {
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
    } else if (tr->phase == TRANS_ARRIVE) {
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
    } else if (tr->phase == TRANS_DESCEND) {
        tr->timer++;
        float t    = (float)tr->timer / TRANS_DESCEND_DUR;
        if (t > 1.0f) t = 1.0f;
        float ease = 1.0f - (1.0f - t) * (1.0f - t);
        p->y  = -100.0f + (player_game_y - (-100.0f)) * ease;
        p->dx = 0.0f;
        p->dy = 0.0f;
        if (tr->timer >= TRANS_DESCEND_DUR) {
            p->y           = player_game_y;
            p->x           = (float)(SCREEN_W / 2 - 30);
            tr->phase      = TRANS_INACTIVE;
            tr->star_speed = 1.0f;
            tr->sol_offset = 0.0f;
            tr->level_changed = 0;
            return 0;
        }
    }
    return 1;
}

void display_transition(Transition *tr, Bitmaps *b, Assets *assets,
                        Player *p, Game *game, Etoile *etoiles, int n_etoiles) {
    clear_bitmap(b->buffer);

    for (int i = 0; i < n_etoiles; i++) {
        int gris = etoiles[i].luminosite;
        putpixel(b->buffer, etoiles[i].x, etoiles[i].y, makecol(gris, gris, gris));
    }

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

    if (p->y > -84.0f && p->y < (float)SCREEN_H)
        draw_player_ship(b->buffer, p, assets);

    if (tr->phase == TRANS_INACTIVE) {
        sync_fond_with_etoiles(b->fond, etoiles, n_etoiles);
        b->fond_scroll_x = 0.0f;
    }

    blit(b->buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

// ── Cinématique de victoire ──────────────────────────────────────────────────

void victory_cinematic_reset(VictoryCinematic *vc) {
    vc->phase         = VICTORY_INACTIVE;
    vc->timer         = 0;
    vc->star_speed    = 1.0f;
    vc->dialogue_done = 0;
}

// Démarre la cinématique : le boss vient d'être tué
void victory_cinematic_start(VictoryCinematic *vc) {
    vc->phase         = VICTORY_BOSS_DYING;
    vc->timer         = 0;
    vc->star_speed    = 1.0f;
    vc->dialogue_done = 0;
}

// Met à jour la cinématique de victoire, retourne 1 tant qu'elle est active
int victory_cinematic_update(VictoryCinematic *vc, Player *p, Boss *boss,
                              Etoile *etoiles, int n_etoiles, float player_game_y) {
    if (vc->phase == VICTORY_INACTIVE) return 0;

    // Avance les étoiles selon la vitesse courante
    for (int i = 0; i < n_etoiles; i++) {
        int dy = (int)(etoiles[i].vitesse * vc->star_speed);
        etoiles[i].y += dy;
        if (etoiles[i].y >= SCREEN_HEIGHT) {
            etoiles[i].y = 0;
            etoiles[i].x = rand() % SCREEN_WIDTH;
        }
    }

    switch (vc->phase) {

        case VICTORY_BOSS_DYING:
            // boss.c gère le tremblement via boss->dying ; on attend death_done
            vc->timer++;
            if (boss->death_done) {
                vc->phase = VICTORY_BOSS_EXPLODING;
                vc->timer = 0;
            }
            break;

        case VICTORY_BOSS_EXPLODING:
            // La grande explosion est rendue dans victory_cinematic_draw
            vc->timer++;
            // Après 50 frames (durée de l'animation d'explosion)
            if (vc->timer >= 50 * 5) {
                vc->phase = VICTORY_WARP_CENTER;
                vc->timer = 0;
                p->dx     = 0.0f;
            }
            break;

        case VICTORY_WARP_CENTER: {
            // Centrage progressif du vaisseau
            vc->timer++;
            float t      = (float)vc->timer / TRANS_CENTER_DUR;
            if (t > 1.0f) t = 1.0f;
            float ease   = 1.0f - (1.0f - t) * (1.0f - t);
            float target = (float)(SCREEN_W / 2 - 30);
            p->x  = p->x + (target - p->x) * ease;
            p->dx = 0.0f;
            if (vc->timer >= TRANS_CENTER_DUR) {
                p->x      = target;
                vc->phase = VICTORY_WARP_ACCEL;
                vc->timer = 0;
            }
            break;
        }

        case VICTORY_WARP_ACCEL:
            // Accélération du parallax des étoiles : simulation de vitesse
            vc->timer++;
            vc->star_speed = 1.0f + ((float)vc->timer / 60.0f) * 5.0f;
            if (vc->star_speed > 8.0f) vc->star_speed = 8.0f;

            // Après 90 frames, passage au dialogue
            if (vc->timer >= 90) {
                vc->phase = VICTORY_DIALOGUE;
                vc->timer = 0;
            }
            break;

        case VICTORY_DIALOGUE:
            // Attente du clic du joueur (géré dans main via victory_cinematic_draw)
            vc->timer++;
            if (vc->dialogue_done) {
                vc->phase = VICTORY_SHIP_EXIT;
                vc->timer = 0;
            }
            break;

        case VICTORY_SHIP_EXIT:
            // Le vaisseau monte et quitte l'écran vers le haut
            vc->timer++;
            p->y -= 8.0f;
            p->dx = 0.0f;
            if (p->y < -100.0f) {
                vc->phase = VICTORY_SCREEN;
                vc->timer = 0;
            }
            break;

        case VICTORY_SCREEN:
            // Écran de victoire : géré par ihm.c, on reste dans cet état
            break;

        default:
            break;
    }
    return 1;
}

// Rendu de la cinématique de victoire
void victory_cinematic_draw(VictoryCinematic *vc, Bitmaps *b, Assets *assets,
                            Player *p, Boss *boss, Etoile *etoiles, int n_etoiles) {
    clear_to_color(b->buffer, makecol(10, 10, 20));

    // Fond étoilé commun à toutes les phases
    for (int i = 0; i < n_etoiles; i++) {
        int gris = etoiles[i].luminosite;
        putpixel(b->buffer, etoiles[i].x, etoiles[i].y, makecol(gris, gris, gris));
    }

    switch (vc->phase) {

        case VICTORY_BOSS_DYING:
            // Le boss tremble avec des mini-explosions
            if (boss->active || boss->dying) {
                int draw_x = (int)(boss->x + boss->shake_x);
                masked_blit(assets->boss, b->buffer, 0, 0, draw_x, boss->y, SCREEN_W, SCREEN_H);

                if (boss->exp_mini_active) {
                    int c   = boss->exp_mini_frame % 10;
                    int row = boss->exp_mini_frame / 10;
                    masked_stretch_blit(assets->explosion1, b->buffer,
                                        c * 100, row * 100, 100, 100,
                                        (int)boss->exp_mini_x, (int)boss->exp_mini_y,
                                        60, 40);
                }
            }
            break;

        case VICTORY_BOSS_EXPLODING: {
            // Grande explosion finale
            int frame = (vc->timer / 5) % 50;
            int c   = frame % 10;
            int row = frame / 10;
            masked_stretch_blit(assets->explosion1, b->buffer,
                                c * 100, row * 100, 100, 100,
                                boss->x, boss->y, 300, 185);
            break;
        }

        case VICTORY_WARP_CENTER:
        case VICTORY_WARP_ACCEL:
            // Vaisseau visible, étoiles en accélération
            if (p->y > -84.0f && p->y < (float)SCREEN_H)
                draw_player_ship(b->buffer, p, assets);
            break;

        case VICTORY_DIALOGUE: {
            // Étoiles rapides + vaisseau centré + dialogue Claude
            draw_player_ship(b->buffer, p, assets);

            // Panneau de dialogue en bas de l'écran
            int px = (int)(SCREEN_W * 0.025f);
            int py = (int)(SCREEN_H * 0.817f);
            stretch_sprite(b->buffer, assets->claude, px, py,
                           (int)(SCREEN_W * 0.14f), (int)(SCREEN_H * 0.17f));

            int bx = (int)(SCREEN_W * 0.188f);
            int by2 = (int)(SCREEN_H * 0.833f);
            int bw = (int)(SCREEN_W * 0.78f);
            int bh = (int)(SCREEN_H * 0.12f);
            rectfill(b->buffer, bx, by2, bx + bw, by2 + bh, makecol(20, 20, 50));
            rect(b->buffer,     bx, by2, bx + bw, by2 + bh, makecol(100, 150, 255));

            // Texte du dialogue
            const char *msg1 = "Bravo, la mission est un succes !";
            const char *msg2 = "[ Cliquez pour continuer ]";
            int tw1 = text_length(font, msg1);
            int tw2 = text_length(font, msg2);
            BITMAP *tmp1 = create_bitmap(tw1, text_height(font));
            BITMAP *tmp2 = create_bitmap(tw2, text_height(font));
            if (tmp1) {
                clear_to_color(tmp1, makecol(255, 0, 255));
                textout_ex(tmp1, font, msg1, 0, 0, makecol(255, 255, 255), -1);
                stretch_sprite(b->buffer, tmp1,
                               bx + 10, by2 + 12,
                               tw1 * 2, text_height(font) * 2);
                destroy_bitmap(tmp1);
            }
            if (tmp2) {
                clear_to_color(tmp2, makecol(255, 0, 255));
                textout_ex(tmp2, font, msg2, 0, 0, makecol(150, 150, 150), -1);
                stretch_sprite(b->buffer, tmp2,
                               bx + bw - tw2 * 2 - 10, by2 + bh - text_height(font) * 2 - 6,
                               tw2 * 2, text_height(font) * 2);
                destroy_bitmap(tmp2);
            }
            break;
        }

        case VICTORY_SHIP_EXIT:
            // Vaisseau remonte et quitte l'écran
            if (p->y > -100.0f)
                draw_player_ship(b->buffer, p, assets);
            break;

        case VICTORY_SCREEN:
            // Rien de spécial : l'écran de victoire est dessiné par ihm.c
            break;

        default:
            break;
    }

    blit(b->buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void gameover_dialogue_reset(GameOverDialogue *d) {
    d->active            = 1;
    d->done              = 0;
    d->timer             = 0;
    d->char_shown        = 0;
    d->phase             = 0;
    d->premier_affichage = 1;
}

int gameover_dialogue_update(GameOverDialogue *d) {
    if (!d->active || d->done) return 0;
    return 1;
}

void gameover_dialogue_draw(BITMAP *buffer, GameOverDialogue *d, Assets *assets,
                             Player *p, Audio *audio) {
    if (!d->active) return;

    int sx[] = {152, 208, 64, 136};
    int sy[] = {336, 328, 368, 416};
    int sw[] = {48, 47, 47, 47};
    int sh[] = {64, 63, 40, 31};

    // Claude en bas à gauche
    stretch_sprite(buffer, assets->claude, LX(0.025), LY(0.817), LX(CLAUDE_L), LY(CLAUDE_H));

    // Vaisseau centré
    int v_w = LX(VAISSEAU_L);
    int v_h = LY(VAISSEAU_H);
    masked_stretch_blit(assets->player_sprites,
                        buffer,
                        sx[p->skin_id], sy[p->skin_id], sw[p->skin_id], sh[p->skin_id],
                        LX(0.5) - v_w/2, LY(0.5) - v_h/2, v_w, v_h);

    // ── Phase 0 ─────────────────────────────────────────────────────────────
    if (d->phase == 0) {
        if (d->premier_affichage) {
            audio_play_speech6(audio);
            d->premier_affichage = 0;
        }
        ecrire_texte(buffer, "Recrue, vous m'entendez ? Vous m'entendez ? ...",
                     LX(0.188), LY(0.850), makecol(255, 255, 255), 1.7);

        if ((mouse_b & 1) || key[KEY_SPACE]) {
            ATTENDRE_RELACHE();
            while (mouse_b & 1) rest(1);
            while (key[KEY_SPACE]) rest(1);
            audio_stop_speech(audio);
            audio_play_speech7(audio);
            d->phase             = 1;
            d->premier_affichage = 0;
        }

    // ── Phase 1 ─────────────────────────────────────────────────────────────
    } else if (d->phase == 1) {
        ecrire_texte(buffer, "Ne vous inquietez pas, nous allons vous rapatrier...",
                     LX(0.188), LY(0.850), makecol(255, 255, 255), 1.7);

        if ((mouse_b & 1) || key[KEY_SPACE]) {
            ATTENDRE_RELACHE();
            while (mouse_b & 1) rest(1);
            while (key[KEY_SPACE]) rest(1);
            audio_stop_speech(audio);
            d->done   = 1;
            d->active = 0;
        }
    }
}