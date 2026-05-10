//
// Created by ruben on 4/2/2026.
//

#ifndef PROJET_ALGO_S2_PLAYER_H
#define PROJET_ALGO_S2_PLAYER_H

#include <allegro.h>
#include "../headers/assets.h"

#define MAX_LASERS 16

//definition de la structure laser
typedef struct Laser {
    float x, y; //position
    float dy, dx; //vitesse
    int active; // flag pour savoir s'il est activé
    int frame; //pour affiché le sprite du laser
    int frame_timer; //défile le sprite
} Laser;

//structure du joueur(vaisseau)
typedef struct Player {
    float x, y;  //position
    float dx, dy;//vitesse
    float ddx, ddy; //acceleration
    float vmax; //vitesse max
    int vies;
    int invincible; //flag
    int invincible_timer; //durée de l'invincibilité
    int skin_id;
    Laser *lasers;
    int laser_count;
    char pseudo[20];
} Player;

void player_init (Player *p); //initialise les différents parametres du vaisseau
void player_update (Player *p); //actualise tous les parametres du vaisseau
void player_move_left (Player *p); //déplace le vaisseau a gauche
void player_move_right (Player *p); //a droite
void player_destroy (Player **p); //libere l'espace alloué dynamiquement pour les lasers
void player_shot (Player *p); //tire un laser

#endif //PROJET_ALGO_S2_PLAYER_H