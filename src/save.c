//
// Created by ruben on 4/16/2026.
//

#include "../headers/save.h"
//
// save.c - Système de sauvegarde
//

#include "../headers/save.h"
#include <stdio.h>
#include <string.h>

// Retourne le chemin valide vers le fichier de sauvegarde, ou NULL
static const char *save_path(void) {
    FILE *f = fopen(SAVE_FILE, "rb");
    if (f) { fclose(f); return SAVE_FILE; }
    f = fopen(SAVE_FILE_ALT, "rb");
    if (f) { fclose(f); return SAVE_FILE_ALT; }
    return NULL;
}

// Retourne le chemin d'écriture (préfère le chemin principal)
static const char *write_path(void) {
    FILE *f = fopen(SAVE_FILE, "wb");
    if (f) { fclose(f); return SAVE_FILE; }
    return SAVE_FILE_ALT;
}

// Vérifie si une sauvegarde valide existe
int save_exists(void) {
    const char *path = save_path();
    if (!path) return 0;

    FILE *f = fopen(path, "rb");
    if (!f) return 0;

    SaveData sd;
    int ok = (fread(&sd, sizeof(SaveData), 1, f) == 1) && (sd.magic == SAVE_MAGIC);
    fclose(f);
    return ok;
}

// Sauvegarde l'état actuel de la partie
// Retourne 1 si succès, 0 si échec
int save_game(const Player *p, const Game *game) {
    SaveData sd;
    sd.magic       = SAVE_MAGIC;
    sd.level_index = game_level_index(game);
    sd.vies        = p->vies;
    sd.skin_id     = p->skin_id;
    strncpy(sd.pseudo, p->pseudo, sizeof(sd.pseudo) - 1);
    sd.pseudo[sizeof(sd.pseudo) - 1] = '\0';

    const char *path = write_path();
    FILE *f = fopen(path, "wb");
    if (!f) return 0;

    int ok = (fwrite(&sd, sizeof(SaveData), 1, f) == 1);
    fclose(f);
    return ok;
}

// Charge une sauvegarde et applique les données au joueur et au jeu
// Retourne 1 si succès, 0 si échec ou sauvegarde corrompue
int load_game(Player *p, Game *game, Boss *boss) {
    const char *path = save_path();
    if (!path) return 0;

    FILE *f = fopen(path, "rb");
    if (!f) return 0;

    SaveData sd;
    if (fread(&sd, sizeof(SaveData), 1, f) != 1 || sd.magic != SAVE_MAGIC) {
        fclose(f);
        return 0;
    }
    fclose(f);

    // Restaure le joueur
    strncpy(p->pseudo, sd.pseudo, sizeof(p->pseudo) - 1);
    p->pseudo[sizeof(p->pseudo) - 1] = '\0';
    p->vies    = sd.vies;
    p->skin_id = sd.skin_id;

    // Avance le jeu jusqu'au bon niveau
    game->current_level = game->levels[0];
    for (int i = 0; i < sd.level_index && i < game->nlevels - 1; i++)
        game_next_level(game, boss);

    return 1;
}

// Supprime le fichier de sauvegarde
void save_delete(void) {
    remove(SAVE_FILE);
    remove(SAVE_FILE_ALT);
}