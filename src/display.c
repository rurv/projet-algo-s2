//
// Created by ruben on 4/2/2026.
//

#include "../headers/display.h"

BITMAP *buffer;

void init_display (void) {
    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    if (!buffer) {
        allegro_message("Erreur création buffer");
        allegro_exit();
        exit(1);
    }
}

void display (void) {
    clear_bitmap(buffer);
    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void destroy_display (void) {
    destroy_bitmap(buffer);
}