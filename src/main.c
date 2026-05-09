#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <allegro.h>

#include "../headers/display.h"
#include "../headers/game.h"
#include "../headers/audio.h"
#include "../headers/ihm.h"
#include "../headers/save.h"

typedef enum {
    TRANS_INACTIVE = 0,
    TRANS_CENTER,
    TRANS_ACCEL,
    TRANS_WARP,
    TRANS_ARRIVE,
    TRANS_DESCEND,
} TransPhase;

static struct {
    TransPhase phase;
    int        timer;
    float      star_speed;
    float      sol_offset;
    float      ship_anim_y;
    int        level_changed; // 1 une fois que game_next_level a été appelé dans WARP
} trans = {TRANS_INACTIVE, 0, 1.0f, 0.0f, 0.0f, 0};

#define TRANS_CENTER_DUR  60
#define TRANS_ACCEL_DUR   90
#define TRANS_WARP_DUR    80   // augmenté pour laisser le temps au vaisseau de sortir
#define TRANS_ARRIVE_DUR  70
#define TRANS_DESCEND_DUR 80

// Redessine bmps.fond à partir du tableau etoiles[] pour éviter la discontinuité
static void sync_fond_with_etoiles(BITMAP *fond, Etoile *etoiles, int n) {
    clear_to_color(fond, makecol(255, 0, 255)); // fond magenta (couleur masque)
    for (int i = 0; i < n; i++) {
        int gris = etoiles[i].luminosite;
        // dessiner un carré 2x2 comme dans init_display
        if (etoiles[i].x + 1 < fond->w && etoiles[i].y + 1 < fond->h) {
            putpixel(fond, etoiles[i].x,     etoiles[i].y,     makecol(gris, gris, gris));
            putpixel(fond, etoiles[i].x + 1, etoiles[i].y,     makecol(gris, gris, gris));
            putpixel(fond, etoiles[i].x,     etoiles[i].y + 1, makecol(gris, gris, gris));
            putpixel(fond, etoiles[i].x + 1, etoiles[i].y + 1, makecol(gris, gris, gris));
        }
    }
}

// Dessine le vaisseau du joueur sur un bitmap donné
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

int main() {
    Bitmaps bmps;
    Assets  assets;
    init_display(&bmps, &assets);

    Player player;
    player_init(&player);

    Boss boss;
    boss_init(&boss);
    boss.active = 0;

    Audio audio;
    audio_init(&audio);
    audio_play_bo(&audio);

    Game game = init_game();
    enum EcranActuel ecran = MENU_PRINCIPAL;
    install_keyboard();
    install_timer();
    install_mouse();

    srand(time(NULL));
    int space_pressed   = 0;
    int boss_dead_sound = 0;
    int s_pressed       = 0;
    int quitter         = 0;

    float player_game_y = (float)(SCREEN_H / 5 * 4);

    Etoile etoiles[NOMBRE_ETOILES];
    for (int i = 0; i < NOMBRE_ETOILES; i++) {
        etoiles[i].x         = rand() % SCREEN_WIDTH;
        etoiles[i].y         = rand() % SCREEN_HEIGHT;
        etoiles[i].vitesse   = 1 + (rand() % 3);
        etoiles[i].luminosite = 100 + (rand() % 155);
    }

    while (!quitter && !key[KEY_ESC]) {

        // ── Mise à jour étoiles (menus et cinématique) ───────────────────────
        if (ecran != JEU || trans.phase != TRANS_INACTIVE) {
            if (ecran != JEU)
                clear_to_color(bmps.buffer, makecol(10, 10, 20));

            for (int i = 0; i < NOMBRE_ETOILES; i++) {
                // Déplacement : pendant la cinématique on applique star_speed,
                // hors cinématique (menus) on avance normalement
                int dy = (trans.phase != TRANS_INACTIVE)
                         ? (int)(etoiles[i].vitesse * trans.star_speed)
                         : etoiles[i].vitesse;
                etoiles[i].y += dy;
                if (etoiles[i].y >= SCREEN_HEIGHT) {
                    etoiles[i].y = 0;
                    etoiles[i].x = rand() % SCREEN_WIDTH;
                }
                int gris = etoiles[i].luminosite;
                if (ecran != JEU)
                    putpixel(bmps.buffer, etoiles[i].x, etoiles[i].y, makecol(gris, gris, gris));
            }
        }

        switch (ecran) {
            case MENU_PRINCIPAL:
                ecran = menu_principale(ecran, bmps.buffer);
                break;
            case SAISIE_PSEUDO:
                ecran = saisie_pseudo(ecran, bmps.buffer, &player);
                break;
            case REGLE:
                ecran = regles(ecran, bmps.buffer);
                break;
            case CHOIX:
                ecran = choix(ecran, bmps.buffer, &assets, &player);
                break;
            case DECOMPTE:
                ecran = decompte(ecran, bmps.buffer, &assets, &player);
                break;

            // ================================================================
            case JEU: {

                // ── Game Over ────────────────────────────────────────────────
                if (player.vies <= 0) {
                    trans.phase      = TRANS_INACTIVE;
                    trans.star_speed = 1.0f;
                    trans.sol_offset = 0.0f;
                    trans.level_changed = 0;
                    ecran = GAME_OVER;
                    break;
                }

                // ── Déclenchement cinématique ────────────────────────────────
                if (game.level_complete && trans.phase == TRANS_INACTIVE) {
                    trans.phase         = TRANS_CENTER;
                    trans.timer         = 0;
                    trans.star_speed    = 1.0f;
                    trans.sol_offset    = 0.0f;
                    trans.level_changed = 0;
                }

                int controls_locked = (trans.phase != TRANS_INACTIVE);

                // ── Machine à états ──────────────────────────────────────────

                if (trans.phase == TRANS_CENTER) {
                    trans.timer++;
                    float t    = (float)trans.timer / TRANS_CENTER_DUR;
                    if (t > 1.0f) t = 1.0f;
                    float ease = 1.0f - (1.0f - t) * (1.0f - t);
                    float target_x = (float)(SCREEN_W / 2 - 30);
                    player.x  = player.x + (target_x - player.x) * ease;
                    player.dx = 0.0f;

                    if (trans.timer >= TRANS_CENTER_DUR) {
                        player.x    = target_x;
                        trans.phase = TRANS_ACCEL;
                        trans.timer = 0;
                    }
                }
                else if (trans.phase == TRANS_ACCEL) {
                    trans.timer++;
                    float t          = (float)trans.timer / TRANS_ACCEL_DUR;
                    trans.star_speed = 1.0f + t * t * 29.0f;
                    trans.sol_offset += trans.star_speed * 2.0f;

                    if (trans.timer >= TRANS_ACCEL_DUR) {
                        trans.phase = TRANS_WARP;
                        trans.timer = 0;
                    }
                }
                else if (trans.phase == TRANS_WARP) {
                    trans.timer++;
                    trans.star_speed = 30.0f;
                    player.y -= 25.0f; // monte hors champ

                    // Changer de niveau dès que le vaisseau est sorti par le haut
                    // (y + hauteur_sprite < 0, soit y < -84)
                    if (!trans.level_changed && player.y < -84.0f) {
                        game_next_level(&game, &boss);
                        game.level_complete = 0;
                        boss_dead_sound     = 0;
                        trans.level_changed = 1;
                        // On remet le vaisseau hors champ en haut pour TRANS_ARRIVE
                        player.x  = (float)(SCREEN_W / 2 - 30);
                        player.y  = -200.0f; // bien au-dessus
                        player.dx = 0.0f;
                        player.dy = 0.0f;
                    }

                    // On attend que le niveau ait changé ET quelques frames de plus
                    // pour laisser les étoiles filer un peu avant d'arriver
                    if (trans.level_changed && trans.timer >= TRANS_WARP_DUR) {
                        trans.phase = TRANS_ARRIVE;
                        trans.timer = 0;
                    }
                }
                else if (trans.phase == TRANS_ARRIVE) {
                    trans.timer++;
                    float t          = (float)trans.timer / TRANS_ARRIVE_DUR;
                    trans.star_speed = 30.0f * (1.0f - t) + 1.0f;
                    float ease_in    = t * t;
                    trans.sol_offset = (float)SCREEN_H * (1.0f - ease_in);

                    if (trans.timer >= TRANS_ARRIVE_DUR) {
                        trans.sol_offset = 0.0f;
                        trans.star_speed = 1.0f;
                        trans.phase      = TRANS_DESCEND;
                        trans.timer      = 0;
                        player.y         = -100.0f;
                    }
                }
                else if (trans.phase == TRANS_DESCEND) {
                    trans.timer++;
                    float t    = (float)trans.timer / TRANS_DESCEND_DUR;
                    if (t > 1.0f) t = 1.0f;
                    float ease = 1.0f - (1.0f - t) * (1.0f - t);
                    player.y  = -100.0f + (player_game_y - (-100.0f)) * ease;
                    player.dx = 0.0f;
                    player.dy = 0.0f;

                    if (trans.timer >= TRANS_DESCEND_DUR) {
                        // ── Synchroniser bmps.fond avec les étoiles actuelles ──
                        // pour que le rendu normal reprenne exactement là où
                        // la cinématique s'est arrêtée, sans saut visible.
                        sync_fond_with_etoiles(bmps.fond, etoiles, NOMBRE_ETOILES);
                        bmps.fond_scroll_x = 0.0f;

                        player.y         = player_game_y;
                        player.x         = (float)(SCREEN_W / 2 - 30);
                        trans.phase      = TRANS_INACTIVE;
                        trans.star_speed = 1.0f;
                        trans.sol_offset = 0.0f;
                        trans.level_changed = 0;
                    }
                }

                // ── Contrôles (bloqués pendant cinématique) ──────────────────
                if (!controls_locked) {
                    if (key[KEY_S] && !s_pressed) { save_game(&player, &game); s_pressed = 1; }
                    if (!key[KEY_S]) s_pressed = 0;

                    if (key[KEY_SPACE]) {
                        if (!space_pressed) {
                            int nb_actifs = 0;
                            for (int i = 0; i < player.laser_count; i++)
                                if (player.lasers[i].active) nb_actifs++;
                            if (nb_actifs == 0) {
                                player_shot(&player);
                                if (game.bonus_actif == TRIPLE_LASER) {
                                    int tirs_supp = 0;
                                    for (int i = 0; i < player.laser_count && tirs_supp < 2; i++) {
                                        if (!player.lasers[i].active) {
                                            player.lasers[i].active = 1;
                                            player.lasers[i].y  = player.y;
                                            player.lasers[i].dy = -25.0f;
                                            player.lasers[i].x  = (tirs_supp == 0) ? player.x + 10 : player.x + 40;
                                            player.lasers[i].dx = (tirs_supp == 0) ? -7.0f : 7.0f;
                                            tirs_supp++;
                                        }
                                    }
                                }
                                audio_play_laser(&audio);
                            }
                            space_pressed = 1;
                        }
                    } else {
                        space_pressed = 0;
                    }
                }

                // ── Mise à jour jeu ──────────────────────────────────────────
                if (!controls_locked) {
                    game_update(&player, &boss, &game, &audio);
                    colision_asteroids_player(&player, &game.am, &audio);
                    if (boss.pv <= 0 && !boss_dead_sound && game_is_boss(&game)) {
                        audio_play_explode(&audio);
                        boss_dead_sound = 1;
                    }
                } else {
                    for (int i = 0; i < player.laser_count; i++)
                        player.lasers[i].active = 0;
                }

                // ── Rendu ────────────────────────────────────────────────────
                if (trans.phase != TRANS_INACTIVE) {
                    clear_bitmap(bmps.buffer);

                    // Fond étoilé depuis le tableau (positions exactes de la cinématique)
                    for (int i = 0; i < NOMBRE_ETOILES; i++) {
                        int gris = etoiles[i].luminosite;
                        putpixel(bmps.buffer, etoiles[i].x, etoiles[i].y,
                                 makecol(gris, gris, gris));
                    }

                    // Sol avec offset
                    if (!game_is_boss(&game)) {
                        int gh  = game_ground_height();
                        int gy  = SCREEN_H - gh + (int)trans.sol_offset;
                        int idx = game_level_index(&game);
                        if (idx >= 3) idx = 2;
                        if (gy < SCREEN_H) {
                            stretch_blit(assets.sol_sprites[idx], bmps.buffer,
                                         0, 0,
                                         assets.sol_sprites[idx]->w,
                                         assets.sol_sprites[idx]->h,
                                         0, gy, SCREEN_W, gh);
                        }
                    }

                    // Vaisseau : visible seulement quand il est dans le champ
                    // (y > -84 pour voir même le haut du sprite, y < SCREEN_H pour le bas)
                    int ship_visible = (player.y > -84.0f && player.y < (float)SCREEN_H);
                    // Pendant WARP on ne force pas ship_visible à 0 :
                    // le vaisseau sort naturellement par le haut grâce à la condition ci-dessus.
                    // Pendant ARRIVE le vaisseau est à -200, donc non visible → correct.
                    if (ship_visible) {
                        draw_player_ship(bmps.buffer, &player, &assets);
                    }

                    blit(bmps.buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                } else {
                    display(&bmps, &assets, &player, &boss, &game);
                    draw_bonus_system(bmps.buffer, &game, &assets);
                }

                rest(1);
                break;
            } // case JEU

            case REPRENDRE:
                if (load_game(&player, &game, &boss))
                    ecran = JEU;
                else
                    ecran = MENU_PRINCIPAL;
                break;

            case GAME_OVER:
                ecran = game_over_screen(ecran, bmps.buffer, &player);
                if (ecran == CHOIX) {
                    player_init(&player);
                    boss_init(&boss);
                    boss.active      = 0;
                    game             = init_game();
                    boss_dead_sound  = 0;
                    trans.phase      = TRANS_INACTIVE;
                    trans.star_speed = 1.0f;
                    trans.sol_offset = 0.0f;
                    trans.level_changed = 0;
                    save_delete();
                }
                break;

            case QUITTER:
                ATTENDRE_RELACHE();
                quitter = 1;
                break;
        }

        if (!quitter && ecran != JEU) {
            show_mouse(bmps.buffer);
            blit(bmps.buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
    }

    audio_destroy(&audio);
    destroy_display(&bmps, &assets);
    allegro_exit();
    return 0;
} END_OF_MAIN();