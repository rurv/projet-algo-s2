#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro.h>

#include "../headers/display.h"
#include "../headers/game.h"
#include "../headers/audio.h"
#include "../headers/ihm.h"

int main() {
    Bitmaps bmps;
    Assets  assets;
    init_display(&bmps, &assets);

    Player player;
    player_init(&player);

    Boss boss;
    boss_init(&boss);
    boss.active = 0; // inactif jusqu'au niveau boss

    Audio audio;
    audio_init(&audio);

    Game game = init_game();
    enum EcranActuel ecran = MENU_PRINCIPAL;
    install_keyboard();
    install_timer();
    install_mouse();

    srand(time(NULL));
    int space_pressed   = 0;
    int boss_dead_sound = 0;

    //Etoiles en fond
    Etoile etoiles[NOMBRE_ETOILES];
    for (int i = 0; i < NOMBRE_ETOILES; i++) {
        etoiles[i].x = rand() % SCREEN_WIDTH;
        etoiles[i].y = rand() % SCREEN_HEIGHT;
        etoiles[i].vitesse = 1 + (rand() % 3);
        etoiles[i].luminosite = 100 + (rand() % 155);
    }

    //boucle
    while (ecran != QUITTER && !key[KEY_ESC]) {

        if (ecran != JEU) {
            // Fond étoilé
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
                ecran = menu_principale(ecran, bmps.buffer );
                break;

            case SAISIE_PSEUDO:
                ecran = saisie_pseudo(ecran, bmps.buffer, &player );
                break;

            case REGLE:
                ecran = regles(ecran, bmps.buffer);
                break;

            case CHOIX:
                ecran = choix(ecran, bmps.buffer,&assets, &player);
                break;

            case DECOMPTE:
                ecran = decompte(ecran, bmps.buffer, &assets, &player);
                break;

            case JEU:
                if (key[KEY_SPACE] && !space_pressed) {
                    player_shot(&player);
                    audio_play_laser(&audio);
                    space_pressed = 1;
                }
                if (!key[KEY_SPACE]) space_pressed = 0;

                game_update(&player, &boss, &game);

                if (boss.pv <= 0 && !boss_dead_sound && game_is_boss(&game)) {
                    audio_play_explode(&audio);
                    boss_dead_sound = 1;
                }

                display(&bmps, &assets, &player, &boss, &game);
                rest(1);

                break;

            case REPRENDRE:
                break;

        }
        if (ecran != JEU) {
            show_mouse(bmps.buffer); // Pour voir ton curseur
            blit(bmps.buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        }
    }

    readkey();
    audio_destroy(&audio);
    destroy_display(&bmps, &assets);
    allegro_exit();
    return 0;
} END_OF_MAIN();
