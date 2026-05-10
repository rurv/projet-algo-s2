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

    a->hit_laser_sfx = load_sample("assets/sfx/hit_laser2.wav");
    if (!a->hit_laser_sfx) a->hit_laser_sfx = load_sample("../assets/sfx/hit_laser2.wav");
    if (!a->hit_laser_sfx) {
        allegro_message("Erreur chargement hit_laser2.wav");
        allegro_exit();
        exit(1);
    }

    a->speech1_sfx = load_sample("assets/sfx/speech1.wav");
    if (!a->speech1_sfx) a->speech1_sfx = load_sample("../assets/sfx/speech1.wav");
    if (!a->speech1_sfx) {
        allegro_message("Erreur chargement speech1.wav");
        allegro_exit();
        exit(1);
    }

    a->speech2_sfx = load_sample("assets/sfx/speech2.wav");
    if (!a->speech2_sfx) a->speech2_sfx = load_sample("../assets/sfx/speech2.wav");
    if (!a->speech2_sfx) {
        allegro_message("Erreur chargement speech2.wav");
        allegro_exit();
        exit(1);
    }

    a->speech3_sfx = load_sample("assets/sfx/speech3.wav");
    if (!a->speech3_sfx) a->speech3_sfx = load_sample("../assets/sfx/speech3.wav");
    if (!a->speech3_sfx) {
        allegro_message("Erreur chargement speech3.wav");
        allegro_exit();
        exit(1);
    }

    a->speech4_sfx = load_sample("assets/sfx/speech4.wav");
    if (!a->speech4_sfx) a->speech4_sfx = load_sample("../assets/sfx/speech4.wav");
    if (!a->speech4_sfx) {
        allegro_message("Erreur chargement speech4.wav");
        allegro_exit();
        exit(1);
    }

    a->speech5_sfx = load_sample("assets/sfx/speech5.wav");
    if (!a->speech5_sfx) a->speech5_sfx = load_sample("../assets/sfx/speech5.wav");
    if (!a->speech5_sfx) {
        allegro_message("Erreur chargement speech5.wav");
        allegro_exit();
        exit(1);
    }

    a->speech6_sfx = load_sample("assets/sfx/speech6.wav");
    if (!a->speech6_sfx) a->speech6_sfx = load_sample("../assets/sfx/speech6.wav");
    if (!a->speech6_sfx) { allegro_message("Erreur chargement speech6.wav"); allegro_exit(); exit(1); }

    a->speech7_sfx = load_sample("assets/sfx/speech7.wav");
    if (!a->speech7_sfx) a->speech7_sfx = load_sample("../assets/sfx/speech7.wav");
    if (!a->speech7_sfx) { allegro_message("Erreur chargement speech7.wav"); allegro_exit(); exit(1); }

    a->speech8_sfx = load_sample("assets/sfx/speech8.wav");
    if (!a->speech8_sfx) a->speech8_sfx = load_sample("../assets/sfx/speech8.wav");
    if (!a->speech8_sfx) { allegro_message("Erreur chargement speech8.wav"); allegro_exit(); exit(1); }
}



void audio_destroy(Audio *a) {
    destroy_sample(a->laser_sfx);
    destroy_sample(a->explode_sfx);
    destroy_sample(a->hit_sfx);
    destroy_sample(a->boss_eclair);
    destroy_sample(a->bo_sfx);
    destroy_sample(a->hit_laser_sfx);
    destroy_sample(a->speech1_sfx);
    destroy_sample(a->speech2_sfx);
    destroy_sample(a->speech3_sfx);
    destroy_sample(a->speech4_sfx);
    destroy_sample(a->speech5_sfx);
    destroy_sample(a->speech6_sfx);
    destroy_sample(a->speech7_sfx);
    destroy_sample(a->speech8_sfx);
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

void audio_play_hit_laser(Audio *a) {
    play_sample(a->hit_laser_sfx, 90, 128, 1000, 0);
}

void audio_play_speech1(Audio *a) { play_sample(a->speech1_sfx, 200, 128, 1000, 0); }
void audio_play_speech2(Audio *a) { play_sample(a->speech2_sfx, 200, 128, 1000, 0); }
void audio_play_speech3(Audio *a) { play_sample(a->speech3_sfx, 200, 128, 1000, 0); }
void audio_play_speech4(Audio *a) { play_sample(a->speech4_sfx, 200, 128, 1000, 0); }
void audio_play_speech5(Audio *a) { play_sample(a->speech5_sfx, 200, 128, 1000, 0); }
void audio_play_speech6(Audio *a) { play_sample(a->speech6_sfx, 200, 128, 1000, 0); }
void audio_play_speech7(Audio *a) { play_sample(a->speech7_sfx, 200, 128, 1000, 0); }
void audio_play_speech8(Audio *a) { play_sample(a->speech8_sfx, 200, 128, 1000, 0); }
void audio_stop_speech(Audio *a) {
    stop_sample(a->speech1_sfx);
    stop_sample(a->speech2_sfx);
    stop_sample(a->speech3_sfx);
    stop_sample(a->speech4_sfx);
    stop_sample(a->speech5_sfx);
    stop_sample(a->speech6_sfx);
    stop_sample(a->speech7_sfx);
    stop_sample(a->speech8_sfx);
}
