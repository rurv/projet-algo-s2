//
// Created by ruben on 3/20/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro.h>

#include "../headers/display.h"

int main() {
    BITMAP *buffer;
    Assets assets;
    init_display(&buffer, &assets);
    srand(time(NULL));
    while (!key[KEY_ESC]) {
        display(buffer, &assets);
    }
    readkey();
    destroy_display(&buffer, &assets);
    allegro_exit();
    return 0;
} END_OF_MAIN();