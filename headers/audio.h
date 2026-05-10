//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_AUDIO_H
#define PROJET_ALGO_S2_AUDIO_H

#include <allegro.h>

// structure qui contient les assets audio
typedef struct Audio {
    SAMPLE *laser_sfx;
    SAMPLE *explode_sfx;
    SAMPLE *hit_sfx;
    SAMPLE *bo_sfx;
    SAMPLE *boss_eclair;
    SAMPLE *hit_laser_sfx;
    SAMPLE *speech1_sfx;
    SAMPLE *speech2_sfx;
    SAMPLE *speech3_sfx;
    SAMPLE *speech4_sfx;
    SAMPLE *speech5_sfx;
} Audio;

void audio_init(Audio *a);              // initialisation de la structure audio et chargements
void audio_destroy(Audio *a);           // liberation des assets audio
void audio_play_laser(Audio *a);        // lecture de l'audio laser
void audio_play_explode(Audio *a);      // lecture de l'audio explosion
void audio_play_hit(Audio *a);          // lecture de l'audio hit
void audio_play_boss_eclair(Audio *a);  // lecture de l'audio eclair du boss
void audio_play_bo(Audio *a);           // lecture de la musique de fond
void audio_stop_bo(Audio *a);           // arret de la musique de fond
void audio_stop_boss_eclair(Audio *a);  // arret de l'audio eclair du boss
void audio_play_hit_laser(Audio *a);    // sfx pour asteroide detruit
void audio_play_speech1(Audio *a);      //les dialogues
void audio_play_speech2(Audio *a);
void audio_play_speech3(Audio *a);
void audio_play_speech4(Audio *a);
void audio_play_speech5(Audio *a);
void audio_stop_speech(Audio *a);       // arrête tous les dialogues

#endif //PROJET_ALGO_S2_AUDIO_H