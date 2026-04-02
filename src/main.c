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
    BITMAP *fond = load_bitmap("fond1.bmp", NULL);
    BITMAP *asteroid = create_bitmap(128, 128);
    BITMAP *sprites_asteroid = load_bitmap("sprites_asteroid2.bmp", NULL);
    if (!sprites_asteroid) {
        allegro_message("probleme de chargement des asteorides");
        allegro_exit();
        return 0;
    }
    int mask = 180;
    int blanc  = makecol(255, 255, 255);
    int magenta = makecol(255, 0, 255);
    for (int y = 0; y < sprites_asteroid->h; y++) {
        for (int x = 0; x < sprites_asteroid->w; x++) {
            if (getr(getpixel(sprites_asteroid, x, y)) > mask && getg(getpixel(sprites_asteroid, x, y)) > mask && getb(getpixel(sprites_asteroid, x, y)) > mask) {
                putpixel(sprites_asteroid, x, y, magenta);
            }
        }
    }
    BITMAP *ship = create_bitmap(48, 64);
    BITMAP *sprites_ship = load_bitmap("modular_ships.bmp", NULL);
    if (!sprites_ship) {
        allegro_message("probleme de chargement des vaisseaux");
        allegro_exit();
        return 0;
    }
    for (int y = 0; y < sprites_ship->h; y++) {
        for (int x = 0; x < sprites_ship->w; x++) {
            if (getr(getpixel(sprites_ship, x, y)) == 0 && getg(getpixel(sprites_ship, x, y)) == 128 && getb(getpixel(sprites_ship, x, y)) == 192) {
                putpixel(sprites_ship, x, y, magenta);
            }
        }
    }

    srand(time(NULL));
    int i = 0;
    int rest_t = 0;
    int mult = 3;

    while (!key[KEY_ESC]) {
        clear_bitmap(buffer);


        if (rest_t % 100 == 0) {
            clear_to_color(asteroid, makecol(255, 0, 255));
            blit(sprites_asteroid, asteroid, i*128, 0, 0, 0, 128, 128);
            i += 1;
            i %= 64;
        }
        masked_blit(asteroid, buffer, 0, 0, SCREEN_W/2, SCREEN_H/2, 128, 128);

        blit(sprites_ship, ship, 208, 326, 0, 0, 48, 64);
        masked_stretch_blit(ship, buffer, 0, 0, 48, 64, SCREEN_W/2 - 100, SCREEN_H/2, 96, 128);

        blit(buffer,screen,0,0,0,0,SCREEN_W,SCREEN_H);
        rest_t++;
    }

    readkey();

    destroy_bitmap(buffer);

    allegro_exit();
    return 0;
} END_OF_MAIN();

void initialisation_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());
    if(set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,1920,1080,0,0)!=0)
    {
        allegro_message("probleme mode graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
}