//
// Created by ruben on 3/20/2026.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro.h>
void initialisation_allegro();
int main(int argc, char *argv[])
{
    int i;
    srand(time(NULL));
    initialisation_allegro();
    allegro_message("Cliquer sur ok pour continuer\n Il faudra appuyer sur une touche pour quitter le programme");
    circlefill(screen,SCREEN_W/2,SCREEN_H/2,100,makecol(255,0,0));
    readkey();
    allegro_exit();
    return 0;
}END_OF_MAIN();

void initialisation_allegro() {
    allegro_init();
    install_keyboard();
    install_mouse();
    set_color_depth(desktop_color_depth());
    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED,800,600,0,0)!=0)
    {
        allegro_message("probleme mode graphique");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
}