//
// Created by ruben on 4/2/2026.
//

#include "../headers/audio.h"

void audio_init(Audio *a) {
    install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL);
    set_volume_per_voice(0);
    reserve_voices(8, 0);
    a->laser_sfx = load_sample("../assets/sfx/laserSFX.wav");
    if (!a->laser_sfx) {
        a->laser_sfx = load_sample("assets/sfx/laserSFX.wav");
        if (!a->laser_sfx) {
            allegro_message("Erreur chargement laserSFX.wav");
            allegro_exit();
            exit(1);
        }
    }
    a->explode_sfx = load_sample("../assets/sfx/explosion.wav");
    if (!a->explode_sfx) {
        a->explode_sfx = load_sample("assets/sfx/explosion.wav");
        if (!a->explode_sfx) {
            allegro_message("Erreur chargement explosion.wav");
            allegro_exit();
            exit(1);
        }
    }
    a->hit_sfx = load_sample("assets/sfx/hit.wav");
    if (!a->hit_sfx) a->hit_sfx = load_sample("../assets/sfx/hit.wav");
    if (!a->hit_sfx) {
        allegro_message("Erreur chargement hit.wav");
        allegro_exit();
        exit(1);
    }
    a->boss_eclair = load_sample("assets/sfx/boss_eclair.wav");
    if (!a->boss_eclair) a->boss_eclair = load_sample("../assets/sfx/boss_eclair.wav");
    if (!a->boss_eclair) {
        allegro_message("Erreur chargement boss_eclair.wav");
        allegro_exit();
        exit(1);
    }
    a->bo_sfx = load_sample("assets/sfx/bo.wav");
    if (!a->bo_sfx) a->bo_sfx = load_sample("../assets/sfx/bo.wav");
    if (!a->bo_sfx) {
        allegro_message("Erreur chargement bo.wav");
        allegro_exit();
        exit(1);
    }
}

void audio_destroy(Audio *a) {
    destroy_sample(a->laser_sfx);
    destroy_sample(a->explode_sfx);
    destroy_sample(a->hit_sfx);
    destroy_sample(a->boss_eclair);
    destroy_sample(a->bo_sfx);
}

void audio_play_laser(Audio *a) {
    play_sample(a->laser_sfx, 75, 128, 1000, 0);
}

void audio_play_explode(Audio *a) {
    play_sample(a->explode_sfx, 125, 128, 1000, 0);
}

void audio_play_hit(Audio *a) {
    play_sample(a->hit_sfx, 100, 128, 1000, 0);
}

void audio_play_boss_eclair(Audio *a) {
    play_sample(a->boss_eclair, 160, 128, 1000, 0);
}

void audio_stop_boss_eclair(Audio *a) {
    stop_sample(a->boss_eclair);
}

void audio_play_bo(Audio *a) {
    play_sample(a->bo_sfx, 170, 128, 1000, 1);
}

void audio_stop_bo(Audio *a) {
    stop_sample(a->bo_sfx);
}
