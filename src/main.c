#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro.h>

#include "../headers/display.h"
#include "../headers/game.h"
#include "../headers/audio.h"
#include "../headers/ihm.h"
#include "../headers/save.h"

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
    int s_pressed = 0;
    int quitter = 0;

    Etoile etoiles[NOMBRE_ETOILES];
    for (int i = 0; i < NOMBRE_ETOILES; i++) {
        etoiles[i].x = rand() % SCREEN_WIDTH;
        etoiles[i].y = rand() % SCREEN_HEIGHT;
        etoiles[i].vitesse = 1 + (rand() % 3);
        etoiles[i].luminosite = 100 + (rand() % 155);
    }

    while (!quitter && !key[KEY_ESC]) {

        if (ecran != JEU) {
            clear_to_color(bmps.buffer, makecol(10, 10, 20));
            for (int i = 0; i < NOMBRE_ETOILES; i++) {
                etoiles[i].y += etoiles[i].vitesse;
                if (etoiles[i].y > SCREEN_HEIGHT) {
                    etoiles[i].y = 0;
                    etoiles[i].x = rand() % SCREEN_WIDTH;
                }
                int gris = etoiles[i].luminosite;
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

            case JEU:
                // Détection game over
                if (player.vies <= 0) {
                    ecran = GAME_OVER;
                    break;
                }

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

                game_update(&player, &boss, &game, &audio);
                colision_asteroids_player(&player, &game.am, &audio);

                if (boss.pv <= 0 && !boss_dead_sound && game_is_boss(&game)) {
                    audio_play_explode(&audio);
                    boss_dead_sound = 1;
                }

                display(&bmps, &assets, &player, &boss, &game);
                draw_bonus_system(bmps.buffer, &game, &assets);
                rest(1);
                break;

            case REPRENDRE:
                if (load_game(&player, &game, &boss))
                    ecran = JEU;
                else
                    ecran = MENU_PRINCIPAL;
                break;

            case GAME_OVER:
                ecran = game_over_screen(ecran, bmps.buffer, &player);
                if (ecran == CHOIX) {
                    // Réinitialisation complète pour une nouvelle partie
                    player_init(&player);
                    boss_init(&boss);
                    boss.active     = 0;
                    game            = init_game();
                    boss_dead_sound = 0;
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