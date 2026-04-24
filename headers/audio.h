//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_AUDIO_H
#define PROJET_ALGO_S2_AUDIO_H
#include <allegro.h>

typedef struct Audio {
    SAMPLE *laser_sfx;
} Audio;

void audio_init(Audio *a);
void audio_destroy(Audio *a);
void audio_play_laser(Audio *a);

#endif //PROJET_ALGO_S2_AUDIO_H