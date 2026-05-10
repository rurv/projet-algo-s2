//
// Created by ruben on 4/16/2026.
//

#include "../headers/save.h"
#include <stdio.h>
#include <string.h>

// Retourne le chemin valide vers le fichier de sauvegarde, ou NULL
static const char *save_path(void) {
    FILE *f = fopen(SAVE_FILE, "r");
    if (f) { fclose(f); return SAVE_FILE; }
    f = fopen(SAVE_FILE_ALT, "r");
    if (f) { fclose(f); return SAVE_FILE_ALT; }
    return NULL;
}

// Retourne le chemin d'écriture (préfère le chemin principal)
static const char *write_path(void) {
    FILE *f = fopen(SAVE_FILE, "w");
    if (f) { fclose(f); return SAVE_FILE; }
    return SAVE_FILE_ALT;
}

// Vérifie si une sauvegarde valide existe
int save_exists(void) {
    const char *path = save_path();
    if (!path) return 0;

    FILE *f = fopen(path, "r");
    if (!f) return 0;

    char magic[8];
    int ok = (fscanf(f, "%7s", magic) == 1) && (strcmp(magic, SAVE_MAGIC) == 0);
    fclose(f);
    return ok;
}

// Sauvegarde l'état actuel de la partie dans un fichier texte lisible
int save_game(const Player *p, const Game *game) {
    const char *path = write_path();
    FILE *f = fopen(path, "w");
    if (!f) return 0; //Retourne 0 si échec

    fprintf(f, "%s\n",  SAVE_MAGIC);
    fprintf(f, "%d\n",  game_level_index(game));
    fprintf(f, "%d\n",  p->vies);
    fprintf(f, "%d\n",  p->skin_id);
    fprintf(f, "%s\n",  p->pseudo);

    fclose(f);
    return 1; // Retourne 1 si succès
}

// Charge une sauvegarde et applique les données au joueur et au jeu

int load_game(Player *p, Game *game, Boss *boss) {
    const char *path = save_path();
    if (!path) return 0;  //retourne 0 si échec

    FILE *f = fopen(path, "r");
    if (!f) return 0;//retourne 0 si échec

    SaveData sd;
    int ok = 1;

    // Lecture ligne par ligne
    if (fscanf(f, "%7s\n",  sd.magic)       != 1) ok = 0;
    if (fscanf(f, "%d\n",  &sd.level_index) != 1) ok = 0;
    if (fscanf(f, "%d\n",  &sd.vies)        != 1) ok = 0;
    if (fscanf(f, "%d\n",  &sd.skin_id)     != 1) ok = 0;
    if (fgets(sd.pseudo, sizeof(sd.pseudo), f) == NULL) ok = 0;
    // Supprimer le \n final si présent
    sd.pseudo[strcspn(sd.pseudo, "\n")] = '\0';

    fclose(f);

    if (!ok || strcmp(sd.magic, SAVE_MAGIC) != 0) return 0;

    // Restaure le joueur
    strncpy(p->pseudo, sd.pseudo, sizeof(p->pseudo) - 1);
    p->pseudo[sizeof(p->pseudo) - 1] = '\0';
    p->vies    = sd.vies;
    p->skin_id = sd.skin_id;

    // Avance le jeu jusqu'au bon niveau
    game->current_level = game->levels[0];
    for (int i = 0; i < sd.level_index && i < game->nlevels - 1; i++)
        game_next_level(game, boss);

    return 1;// Retourne 1 si succès
}

// Supprime le fichier de sauvegarde
void save_delete(void) {
    remove(SAVE_FILE);
    remove(SAVE_FILE_ALT);
}