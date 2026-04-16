//
// Created by ruben on 4/2/2026.
//

#include "../headers/player.h"

Player init_player (void) {
    Player p;
    p.skin_id = 0;
    p.x = SCREEN_W/2;
    p.y = SCREEN_H/2;
    return p;
}

void draw_player (BITMAP** buffer, Assets assets, Player p) {
    BITMAP *sprite = create_bitmap(48, 64);
    clear_to_color(sprite, makecol(255, 0, 255));
    blit(sprite, *buffer, 152, 336, 0, 0, 48, 64);
    draw_sprite(*buffer, sprite, p.x, p.y);
    destroy_bitmap(sprite);

}
