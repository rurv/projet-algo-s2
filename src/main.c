//
// Created by ruben on 3/20/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro.h>

#include "../headers/display.h"

int main() {
    Bitmaps bmps;
    Assets assets;
    init_display(&bmps, &assets);

    srand(time(NULL));
    while (!key[KEY_ESC]) {
        display(&bmps, &assets);
    }
    readkey();
    destroy_display(&bmps, &assets);
    allegro_exit();
    return 0;
} END_OF_MAIN();