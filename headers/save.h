//
// Created by ruben on 4/16/2026.

#ifndef PROJET_ALGO_S2_SAVE_H
#define PROJET_ALGO_S2_SAVE_H

#include "player.h"
#include "game.h"
#include "boss.h"

#define SAVE_FILE     "data/save.txt"
#define SAVE_FILE_ALT "../data/save.txt"
#define SAVE_MAGIC    "ORBS"

typedef struct SaveData {
    char         magic[8];    // Chaine magique pour valider que le fichier c'est bien celui du jeu
    int          level_index; // Index du niveau en cours
    char         pseudo[20];  // Pseudo du joueur
    int          vies;        // Nombre de coeurs restants
    int          skin_id;     // ID du skin de vaisseau choisi
} SaveData;

int  save_exists(void); // Vérifie si une sauvegarde valide existe
int  save_game(const Player *p, const Game *game); //sauvegarde la game
int  load_game(Player *p, Game *game, Boss *boss); //charge la partie sauvegardé
void save_delete(void); //supprime la sauvegarde

#endif //PROJET_ALGO_S2_SAVE_H