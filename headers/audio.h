//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_AUDIO_H
#define PROJET_ALGO_S2_AUDIO_H
#include <allegro.h>

typedef struct Audio {
    SAMPLE *laser_sfx;
    SAMPLE *explode_sfx;
    SAMPLE *hit_sfx;
    SAMPLE *bo_sfx;
    SAMPLE *boss_eclair;
} Audio;

void audio_init(Audio *a);
void audio_destroy(Audio *a);
void audio_play_laser(Audio *a);
void audio_play_explode(Audio *a);
void audio_play_hit(Audio *a);
void audio_play_boss_eclair(Audio *a);
void audio_play_bo(Audio *a);
void audio_stop_bo(Audio *a);
void audio_stop_boss_eclair(Audio *a);

#endif //PROJET_ALGO_S2_AUDIO_H