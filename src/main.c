#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro.h>

#include "../headers/display.h"
#include "../headers/game.h"
#include "../headers/audio.h"

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

    srand(time(NULL));
    int space_pressed   = 0;
    int boss_dead_sound = 0;

    while (!key[KEY_ESC]) {
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
    }

    readkey();
    audio_destroy(&audio);
    destroy_display(&bmps, &assets);
    allegro_exit();
    return 0;
} END_OF_MAIN();
