//
// Created by ruben on 3/20/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro.h>

#include "../headers/display.h"
#include "../headers/game.h"
#include "../headers/audio.h"

int main() {
    Bitmaps bmps;
    Assets assets;
    init_display(&bmps, &assets);
    Player player;
    player_init(&player);
    Audio audio;
    audio_init(&audio);

    srand(time(NULL));
    int space_pressed = 0;
    while (!key[KEY_ESC]) {
        if (key[KEY_SPACE] && !space_pressed) {
            player_shot(&player);
            audio_play_laser(&audio);
            space_pressed = 1;
        }
        if (!key[KEY_SPACE]) space_pressed = 0;
        game_update (&player);
        display(&bmps, &assets, &player);

    }
    readkey();
    audio_destroy(&audio);
    destroy_display(&bmps, &assets);
    allegro_exit();
    return 0;
} END_OF_MAIN();