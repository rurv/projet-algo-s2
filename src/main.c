//
// Created by ruben on 3/20/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro.h>

#include "../headers/display.h"

void initialisation_allegro();

int main() {
    initialisation_allegro();
    init_display();
    srand(time(NULL));
    while (!key[KEY_ESC]) {
        display();
    }
    readkey();
    destroy_display();
    allegro_exit();
    return 0;
} END_OF_MAIN();

void initialisation_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());
    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED,960,540,0,0)!=0)
    {
        allegro_message("probleme mode graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
}