//
// Created by ruben on 3/20/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro.h>

#include "../headers/display.h"

typedef struct Bitmaps {
    BITMAP *buffer;
    BITMAP *ship;
    BITMAP *fond;
    BITMAP *asteroid;
} Bitmaps;

int main() {
    Bitmaps bmps;
    Assets assets;
    init_display(&bmps.buffer, &assets);
    Player player = init_player();
    bmps.ship = player_sprite(assets);
    srand(time(NULL));
    while (!key[KEY_ESC]) {
        display(bmps.buffer, &assets, &player);
    }
    readkey();
    destroy_display(&bmps.buffer, &assets);
    allegro_exit();
    return 0;
} END_OF_MAIN();