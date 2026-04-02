//
// Created by ruben on 3/20/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro.h>

void initialisation_allegro();

int main() {
    initialisation_allegro();
    BITMAP *buffer = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP *perso = create_bitmap(101, 101);
    BITMAP *decor = create_bitmap(SCREEN_W,SCREEN_H);
    BITMAP *sol = create_bitmap(SCREEN_W, SCREEN_H/2);
    BITMAP *trampoline = create_bitmap(150, 30);
    float x = SCREEN_W/2 - 50, y = SCREEN_H/2 - 101;
    float g = 0;
    int depx = 3;
    srand(time(NULL));
    while (!key[KEY_ESC]) {
        y += g;
        if (y >= 2*SCREEN_H/3 - 101) {
            y = 2*SCREEN_H/3 - 101;
            g = 0.0;
        } else {
            g += 0.3;
        }

        clear_bitmap(buffer);
        clear_to_color(decor, makecol(153, 204, 255));
        rectfill(sol, 0, 0, SCREEN_W, SCREEN_H/2, makecol(76, 153, 0));
        blit(sol, decor, 0, 0, 0, 2*SCREEN_H/3, SCREEN_W, SCREEN_H/2);
        rectfill(trampoline, 0, 0, 150, 30, makecol(255, 128, 0));
        blit(trampoline, decor, 0, 0, 3*SCREEN_W/4, 2*SCREEN_H/3, 150, 30);
        blit(decor, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        clear_to_color(perso, makecol(255, 0, 255));
        triangle(perso, 0, 101, 101, 101, 50, 0, makecol(255, 0, 0));
        masked_blit(perso, buffer, 0, 0, (int)x, (int)y, 101, 101);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

        int au_sol = y >= 2*SCREEN_H/3 - 101;

        if (getpixel(screen, x + 50, y + 102) == makecol(255, 128, 0)) g -= 20.0;

        if (key[KEY_LEFT]) {
            if (x - depx < -101) x = SCREEN_W;
            else x -= depx;
        }
        if (key[KEY_RIGHT]) {
            if (x + depx > SCREEN_W) x = -101;
            else x += depx;
        }
        if (key[KEY_UP] && au_sol) {
            g = -15.0;
        }
    }
    readkey();
    destroy_bitmap(perso);
    destroy_bitmap(sol);
    destroy_bitmap(decor);
    destroy_bitmap(buffer);
    allegro_exit();
    return 0;
} END_OF_MAIN();

void initialisation_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());
    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED,1500,900,0,0)!=0)
    {
        allegro_message("probleme mode graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
}