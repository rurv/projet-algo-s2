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

// ── Phases de la cinématique inter-niveaux ──────────────────────────────────
typedef enum {
    TRANS_INACTIVE = 0,
    TRANS_CENTER,       // 1. Le vaisseau glisse vers le centre
    TRANS_ACCEL,        // 2. Accélération : sol descend, étoiles filent vers le bas
    TRANS_WARP,         // 3. Caméra lâche le vaisseau, il sort par le haut
    TRANS_ARRIVE,       // 4. Nouveau sol monte depuis le bas, étoiles se calment
    TRANS_DESCEND,      // 5. Vaisseau entre par le haut et descend vers sa position
} TransPhase;

static struct {
    TransPhase phase;
    int        timer;          // compteur de frames dans la phase courante
    float      star_speed;     // multiplicateur de vitesse des étoiles
    float      sol_offset;     // décalage Y du sol (positif = vers le bas = hors écran)
    float      ship_anim_y;    // Y du vaisseau pendant TRANS_DESCEND
} trans = {TRANS_INACTIVE, 0, 1.0f, 0.0f, 0.0f};

// Durées en frames (≈60 fps)
#define TRANS_CENTER_DUR  60   // 1 s pour centrer
#define TRANS_ACCEL_DUR   90   // 1.5 s d'accélération
#define TRANS_WARP_DUR    40   // ~0.7 s vaisseau hors champ
#define TRANS_ARRIVE_DUR  70   // ~1.2 s le sol arrive
#define TRANS_DESCEND_DUR 80   // ~1.3 s descente du vaisseau

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

    // Position cible du vaisseau au gameplay (calculée une fois)
    float player_game_y = (float)(SCREEN_H / 5 * 4);

    Etoile etoiles[NOMBRE_ETOILES];
    for (int i = 0; i < NOMBRE_ETOILES; i++) {
        etoiles[i].x        = rand() % SCREEN_WIDTH;
        etoiles[i].y        = rand() % SCREEN_HEIGHT;
        etoiles[i].vitesse  = 1 + (rand() % 3);
        etoiles[i].luminosite = 100 + (rand() % 155);
    }

    while (!quitter && !key[KEY_ESC]) {

        // ── Fond étoilé (hors JEU, ou pendant la transition) ────────────────
        if (ecran != JEU || trans.phase != TRANS_INACTIVE) {
            if (ecran != JEU) {
                clear_to_color(bmps.buffer, makecol(10, 10, 20));
            }
            for (int i = 0; i < NOMBRE_ETOILES; i++) {
                etoiles[i].y += (int)(etoiles[i].vitesse * trans.star_speed);
                if (etoiles[i].y > SCREEN_HEIGHT) {
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

                // ── Détection Game Over ──────────────────────────────────────
                if (player.vies <= 0) {
                    trans.phase = TRANS_INACTIVE;
                    trans.star_speed = 1.0f;
                    trans.sol_offset = 0.0f;
                    ecran = GAME_OVER;
                    break;
                }

                // ── Déclenchement de la transition ───────────────────────────
                if (game.level_complete && trans.phase == TRANS_INACTIVE) {
                    trans.phase      = TRANS_CENTER;
                    trans.timer      = 0;
                    trans.star_speed = 1.0f;
                    trans.sol_offset = 0.0f;
                }

                // ── Machine à états de la cinématique ────────────────────────
                int controls_locked = (trans.phase != TRANS_INACTIVE);

                if (trans.phase == TRANS_CENTER) {
                    // Glissement fluide vers le centre (ease in-out)
                    trans.timer++;
                    float t = (float)trans.timer / TRANS_CENTER_DUR;
                    if (t > 1.0f) t = 1.0f;
                    // ease out : t = 1 - (1-t)^2
                    float ease = 1.0f - (1.0f - t) * (1.0f - t);
                    float target_x = (float)(SCREEN_W / 2 - 30); // centre - demi-largeur vaisseau
                    player.x = player.x + (target_x - player.x) * ease;
                    player.dx = 0.0f;

                    if (trans.timer >= TRANS_CENTER_DUR) {
                        player.x = target_x; // forcer position exacte
                        trans.phase = TRANS_ACCEL;
                        trans.timer = 0;
                    }
                }
                else if (trans.phase == TRANS_ACCEL) {
                    trans.timer++;
                    float t = (float)trans.timer / TRANS_ACCEL_DUR;
                    // accélération progressive des étoiles
                    trans.star_speed = 1.0f + t * t * 29.0f; // de 1x à 30x
                    // le sol glisse vers le bas (hors écran)
                    trans.sol_offset += trans.star_speed * 2.0f;

                    if (trans.timer >= TRANS_ACCEL_DUR) {
                        trans.phase = TRANS_WARP;
                        trans.timer = 0;
                    }
                }
                else if (trans.phase == TRANS_WARP) {
                    trans.timer++;
                    // étoiles à pleine vitesse, vaisseau monte hors champ
                    trans.star_speed = 30.0f;
                    player.y -= 25.0f; // sort par le haut

                    if (trans.timer >= TRANS_WARP_DUR) {
                        // Changer de niveau maintenant
                        game_next_level(&game, &boss);
                        game.level_complete = 0;
                        boss_dead_sound = 0;
                        // Repositionner le vaisseau hors champ en haut, centré
                        player.x = (float)(SCREEN_W / 2 - 30);
                        player.y = -100.0f;
                        player.dx = 0.0f;
                        player.dy = 0.0f;
                        trans.ship_anim_y = -100.0f;
                        trans.phase = TRANS_ARRIVE;
                        trans.timer = 0;
                    }
                }
                else if (trans.phase == TRANS_ARRIVE) {
                    trans.timer++;
                    float t = (float)trans.timer / TRANS_ARRIVE_DUR;
                    // décélération des étoiles
                    trans.star_speed = 30.0f * (1.0f - t) + 1.0f;
                    // le sol du nouveau niveau monte depuis le bas
                    // on l'affiche décalé vers le bas et il remonte progressivement
                    float ease_in = t * t; // accélère l'entrée du sol
                    trans.sol_offset = (float)SCREEN_H * (1.0f - ease_in);

                    if (trans.timer >= TRANS_ARRIVE_DUR) {
                        trans.sol_offset = 0.0f;
                        trans.star_speed = 1.0f;
                        trans.phase = TRANS_DESCEND;
                        trans.timer = 0;
                        trans.ship_anim_y = -100.0f;
                        player.y = -100.0f;
                    }
                }
                else if (trans.phase == TRANS_DESCEND) {
                    trans.timer++;
                    float t = (float)trans.timer / TRANS_DESCEND_DUR;
                    if (t > 1.0f) t = 1.0f;
                    // ease out : descente qui ralentit
                    float ease = 1.0f - (1.0f - t) * (1.0f - t);
                    player.y = -100.0f + (player_game_y - (-100.0f)) * ease;
                    player.dx = 0.0f;
                    player.dy = 0.0f;

                    if (trans.timer >= TRANS_DESCEND_DUR) {
                        player.y = player_game_y; // position finale exacte
                        player.x = (float)(SCREEN_W / 2 - 30);
                        trans.phase = TRANS_INACTIVE;
                        trans.star_speed = 1.0f;
                        trans.sol_offset = 0.0f;
                    }
                }

                // ── Contrôles (bloqués pendant cinématique) ──────────────────
                if (!controls_locked) {
                    if (key[KEY_S] && !s_pressed) {
                        save_game(&player, &game);
                        s_pressed = 1;
                    }
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
                                            if (tirs_supp == 0) {
                                                player.lasers[i].x  = player.x + 10;
                                                player.lasers[i].dx = -7.0f;
                                            } else {
                                                player.lasers[i].x  = player.x + 40;
                                                player.lasers[i].dx =  7.0f;
                                            }
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

                // ── Mise à jour jeu (bloquée pendant cinématique) ────────────
                if (!controls_locked) {
                    game_update(&player, &boss, &game, &audio);
                    colision_asteroids_player(&player, &game.am, &audio);

                    if (boss.pv <= 0 && !boss_dead_sound && game_is_boss(&game)) {
                        audio_play_explode(&audio);
                        boss_dead_sound = 1;
                    }
                } else {
                    // Pendant la cinématique : on met quand même à jour les étoiles du fond
                    // et les lasers (pour qu'ils disparaissent naturellement)
                    for (int i = 0; i < player.laser_count; i++)
                        player.lasers[i].active = 0; // nettoyer les lasers
                }

                // ── Rendu ────────────────────────────────────────────────────
                // Pendant la cinématique on dessine manuellement pour gérer sol_offset
                if (trans.phase != TRANS_INACTIVE) {
                    clear_bitmap(bmps.buffer);

                    // Fond étoilé (déjà mis à jour plus haut, on le redessine ici)
                    for (int i = 0; i < NOMBRE_ETOILES; i++) {
                        int gris = etoiles[i].luminosite;
                        putpixel(bmps.buffer, etoiles[i].x, etoiles[i].y,
                                 makecol(gris, gris, gris));
                    }

                    // Sol avec offset (uniquement niveaux non-boss)
                    if (!game_is_boss(&game)) {
                        int gh  = game_ground_height();
                        int gy  = SCREEN_H - gh + (int)trans.sol_offset;
                        int idx = game_level_index(&game);
                        if (idx >= 3) idx = 2;
                        if (gy < SCREEN_H) { // ne dessiner que si visible
                            stretch_blit(assets.sol_sprites[idx], bmps.buffer,
                                         0, 0,
                                         assets.sol_sprites[idx]->w,
                                         assets.sol_sprites[idx]->h,
                                         0, gy, SCREEN_W, gh);
                        }
                    }

                    // Vaisseau (pendant TRANS_CENTER, TRANS_ACCEL, TRANS_DESCEND)
                    if (trans.phase != TRANS_WARP && trans.phase != TRANS_ARRIVE) {
                        int skin_x = 0, skin_y = 0, skin_w = 0, skin_h = 0;
                        switch (player.skin_id) {
                            case VAISSEAU1: skin_x=152; skin_y=336; skin_w=48; skin_h=64; break;
                            case VAISSEAU2: skin_x=208; skin_y=328; skin_w=47; skin_h=63; break;
                            case VAISSEAU3: skin_x= 64; skin_y=368; skin_w=47; skin_h=40; break;
                            case VAISSEAU4: skin_x=136; skin_y=416; skin_w=47; skin_h=31; break;
                        }
                        if (player.y > -90 && player.y < SCREEN_H + 10) {
                            BITMAP *sub = create_sub_bitmap(assets.player_sprites,
                                                            skin_x, skin_y, skin_w, skin_h);
                            stretch_sprite(bmps.buffer, sub, (int)player.x, (int)player.y, 60, 84);
                            destroy_bitmap(sub);
                        }
                    }

                    blit(bmps.buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
                } else {
                    // Rendu normal
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
                    boss.active     = 0;
                    game            = init_game();
                    boss_dead_sound = 0;
                    trans.phase     = TRANS_INACTIVE;
                    trans.star_speed = 1.0f;
                    trans.sol_offset = 0.0f;
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