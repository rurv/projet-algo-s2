//
// Created by ruben on 4/2/2026.
//

#include "../headers/audio.h"

void audio_init(Audio *a) {
    install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);
    a->laser_sfx = load_sample("C:/Users/Ghiwane/CLionProjects/projet-algo--s2/assets/sfx/laserSFX.wav");
    if (!a->laser_sfx) {
        allegro_message("Erreur chargement laserSFX.wav");
        allegro_exit();
        exit(1);
    }
}

void audio_destroy(Audio *a) {
    destroy_sample(a->laser_sfx);
}

void audio_play_laser(Audio *a) {
    play_sample(a->laser_sfx, 200, 128, 1000, 0);
    //                         ^      ^     ^    ^ loop (0=non)
    //                         vol  pan  freq
}
