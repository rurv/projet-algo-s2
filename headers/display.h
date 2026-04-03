//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_DISPLAY_H
#define PROJET_ALGO_S2_DISPLAY_H

#include <allegro.h>

#include "assets.h"

void initialisation_allegro();  // initialisation de allegro
void init_display(BITMAP **buffer, Assets *assets);   // initialisation du buffer et des assets
void display(BITMAP *buffer, Assets *assets);         // affichage
void destroy_display(BITMAP **buffer, Assets *assets); // liberation du buffer

#endif //PROJET_ALGO_S2_DISPLAY_H
