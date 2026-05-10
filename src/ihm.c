//
// Created by ruben on 4/2/2026.
//

#include "../headers/ihm.h"
#include <stdio.h>

void ecrire_centre_texte(BITMAP *dest, const char *texte, int centre_x, int y, int couleur, float multiplicateur) {
    int w = text_length(font, texte);
    int h = text_height(font);
    BITMAP *temp = create_bitmap(w, h);
    if (!temp) return;
    clear_to_color(temp, makecol(255, 0, 255));
    textout_ex(temp, font, texte, 0, 0, couleur, -1);
    stretch_sprite(dest, temp, centre_x - (w * multiplicateur) / 2, y, w * multiplicateur, h * multiplicateur);
    destroy_bitmap(temp);
}

void ecrire_texte(BITMAP *dest, const char *texte, int x, int y, int couleur, float multiplicateur) {
    int w = text_length(font, texte);
    int h = text_height(font);
    BITMAP *temp = create_bitmap(w, h);
    clear_to_color(temp, makecol(255, 0, 255));
    textout_ex(temp, font, texte, 0, 0, couleur, -1);
    stretch_sprite(dest, temp, x, y, w * multiplicateur, h * multiplicateur);
    destroy_bitmap(temp);
}

enum EcranActuel menu_principale(enum EcranActuel ecran, BITMAP *buffer ) {

    ecrire_centre_texte(buffer, "ORBITAL BREAKER",SCREEN_WIDTH/2, LY(0.133), makecol(100, 150, 255), 10);
    ecrire_centre_texte(buffer, "CLAUDE'S LAST MISSION",SCREEN_WIDTH/2, LY(0.233), makecol(219, 103, 33), 5);

    // (~31% à 69% en X, ~33% à 43% en Y)
    rectfill(buffer, LX(0.3125), LY(0.333), LX(0.6875), LY(0.433), makecol(40, 50, 100));
    ecrire_centre_texte(buffer, "Nouvelle partie",SCREEN_WIDTH/2, LY(0.367), makecol(255, 255, 255), 4);


    rectfill(buffer, LX(0.3125), LY(0.467), LX(0.6875), LY(0.567), makecol(30, 30, 50));
    ecrire_centre_texte(buffer, "Reprendre",SCREEN_WIDTH/2, LY(0.500), makecol(200, 200, 200), 4);


    rectfill(buffer, LX(0.3125), LY(0.600), LX(0.6875), LY(0.700), makecol(30, 30, 50));
    ecrire_centre_texte(buffer, "Regles",SCREEN_WIDTH/2, LY(0.633), makecol(200, 200, 200), 4);


    rectfill(buffer, LX(0.3125), LY(0.733), LX(0.6875), LY(0.800), makecol(30, 30, 50));
    ecrire_centre_texte(buffer, "Quitter",
        SCREEN_WIDTH/2, LY(0.750), makecol(200, 200, 200), 4);


    if (mouse_b & 1) {
        if (mouse_x > LX(0.3125) && mouse_x < LX(0.6875) &&
            mouse_y > LY(0.333)  && mouse_y < LY(0.433)) {
            return SAISIE_PSEUDO;


            } else if (mouse_x > LX(0.3125) && mouse_x < LX(0.6875) &&
                       mouse_y > LY(0.467)  && mouse_y < LY(0.567)) {
                return REPRENDRE;

                       } else if (mouse_x > LX(0.3125) && mouse_x < LX(0.6875) &&
                                  mouse_y > LY(0.600)  && mouse_y < LY(0.700)) {
                           return REGLE;

                                  } else if (mouse_x > LX(0.3125) && mouse_x < LX(0.6875) &&
                                             mouse_y > LY(0.733)  && mouse_y < LY(0.800)) {
                                      return QUITTER;

                                             }
        return ecran;
        }
    return ecran;
}


enum EcranActuel saisie_pseudo(enum EcranActuel ecran, BITMAP *buffer, Player *p ) {

    static int longueur_pseudo = 0;

    ecrire_centre_texte(buffer, "Entrez votre pseudo",SCREEN_WIDTH/2, LY(0.250), makecol(255, 255, 255), 3);

    // Boîte de texte (~25% à 75% en X, ~42% à 52% en Y)
    rect(buffer, LX(0.30), LY(0.417), LX(0.7), LY(0.517), makecol(100, 150, 255));

    if (keypressed()) {
        int touche   = readkey();
        int ascii    = touche & 0xff;
        int scancode = touche >> 8;

        if (scancode == KEY_BACKSPACE && longueur_pseudo > 0) {
            longueur_pseudo--;
            p->pseudo[longueur_pseudo] = '\0';
        } else if (scancode == KEY_SPACE) {
            return MENU_PRINCIPAL;
        } else if (scancode == KEY_ENTER && longueur_pseudo > 0) {
            while(key[KEY_ENTER]) {
                rest(1);
            }
            return CHOIX;
        } else if (ascii >= 32 && ascii <= 126 && longueur_pseudo < 18) {
            p->pseudo[longueur_pseudo] = ascii;
            longueur_pseudo++;
            p->pseudo[longueur_pseudo] = '\0';
        }
    }

    char texte_affichage[25];
    sprintf(texte_affichage, "%s_", p->pseudo);
    ecrire_centre_texte(buffer, texte_affichage,SCREEN_WIDTH/2, LY(0.450), makecol(255, 255, 0), 3);

    ecrire_centre_texte(buffer, "(Appuyez sur Entree pour valider)",SCREEN_WIDTH/2, LY(0.667), makecol(150, 150, 150), 2);
    return ecran;
}

enum EcranActuel regles(enum EcranActuel ecran, BITMAP *buffer ) {
    // Panneau principal : ~12% à 88% en X, ~33% à 92% en Y
    rectfill(buffer, LX(0.100), LY(0.333), LX(0.875), LY(0.917), makecol(40, 50, 100));

    // Bouton retour : ~33% à 67% en X, ~82% à 90% en Y
    rectfill(buffer, LX(0.45), LY(0.817), LX(0.55), LY(0.900), makecol(150, 50, 50));

    ecrire_centre_texte(buffer, "REGLES DU JEU",SCREEN_WIDTH/2, LY(0.160), makecol(100, 200, 100), 8);

    ecrire_texte(buffer,"Regle n°1 : Les deplacements",250,LY(0.350),makecol(200, 200, 200), 2);

    ecrire_texte(buffer, " - Q : deplacement du vaisseau vers la gauche",LX(0.131), LY(0.383), makecol(200, 200, 200), 1);
    ecrire_texte(buffer, " - D : deplacement du vaisseau vers la droite",LX(0.131), LY(0.408), makecol(200, 200, 200), 1);

    ecrire_centre_texte(buffer, "Regle n°2 : Fonctionnement d'une partie",SCREEN_WIDTH/2, LY(0.441), makecol(200, 200, 200), 2);
    ecrire_texte(buffer, ".................",LX(0.131), LY(0.474), makecol(200, 200, 200), 1);

    ecrire_centre_texte(buffer, "RETOUR",SCREEN_WIDTH/2, LY(0.850), makecol(255, 255, 255), 2);

    if (mouse_b & 1) {
        if (mouse_x > LX(0.45) && mouse_x < LX(0.55) &&
            mouse_y > LY(0.817) && mouse_y < LY(0.900)) {

            ATTENDRE_RELACHE();
            while(mouse_b & 1) rest(1);

            return MENU_PRINCIPAL;
            }
    }

    return ecran;
}

enum EcranActuel choix(enum EcranActuel ecran, BITMAP *buffer, Assets *assets, Player *p) {
    ecrire_centre_texte(buffer, "Choisissez votre Vaisseau",SCREEN_WIDTH/2, LY(0.250), makecol(255, 255, 255), 3);
            ecrire_centre_texte(buffer, "(Il n'y aura aucun impact sur sur le gameplay)",SCREEN_WIDTH/2, LY(0.667), makecol(150, 150, 150), 2);

            masked_stretch_blit(assets->player_sprites, buffer, 152, 336, 48, 64, LX(0.2), LY(0.5), 60, 84);
            masked_stretch_blit(assets->player_sprites, buffer, 208, 328, 47, 63, LX(0.4), LY(0.5), 60, 84);
            masked_stretch_blit(assets->player_sprites, buffer, 64, 368, 47, 40, LX(0.6), LY(0.5), 60, 84);
            masked_stretch_blit(assets->player_sprites, buffer, 136, 416, 47, 31, LX(0.8), LY(0.5), 60, 84);


            if (mouse_b & 1) {
                if (mouse_x > LX(0.2)  && mouse_x < LX(0.2)+LX(VAISSEAU_L) &&
                    mouse_y > LY(0.5)  && mouse_y < LY(0.5)+LY(VAISSEAU_H)) {
                    p->skin_id = VAISSEAU1;
                    ATTENDRE_RELACHE();
                    return DECOMPTE;

                    } else if (mouse_x > LX(0.4) && mouse_x < LX(0.4)+LX(VAISSEAU_L) &&
                               mouse_y > LY(0.5)  && mouse_y < LY(0.5)+LY(VAISSEAU_H)) {
                        p->skin_id = VAISSEAU2;
                        ATTENDRE_RELACHE();
                        return DECOMPTE;

                               } else if (mouse_x > LX(0.6) && mouse_x < LX(0.6)+LX(VAISSEAU_L) &&
                                          mouse_y > LY(0.5)  && mouse_y < LY(0.5)+LY(VAISSEAU_H)) {
                                   p->skin_id = VAISSEAU3;
                                   ATTENDRE_RELACHE();
                                   return DECOMPTE;

                                          } else if (mouse_x > LX(0.8) && mouse_x < LX(0.8)+LX(VAISSEAU_L) &&
                                                     mouse_y > LY(0.5)  && mouse_y < LY(0.5)+LY(VAISSEAU_H)) {
                                              p->skin_id = VAISSEAU4;
                                              ATTENDRE_RELACHE();
                                              return DECOMPTE;
                                                     }
            }
    return ecran;
        }

enum EcranActuel decompte(enum EcranActuel ecran, BITMAP *buffer, Assets *assets, Player *p, Audio *audio) {
    static enum DiferrentDecompte etat_decompte;
    static int premier_affichage = 1;
    int sx[] = {152, 208, 64, 136};
    int sy[] = {336, 328, 368, 416};
    int sw[] = {48, 47, 47, 47};
    int sh[] = {64, 63, 40, 31};
    if (etat_decompte == DECOMPTE1) {

        if (premier_affichage) {
            audio_play_speech1(audio);
            premier_affichage = 0;
        }

            // Personnage en bas à gauche : ~2% en X, ~82% en Y
            stretch_sprite(buffer, assets->claude, LX(0.025), LY(0.817), LX(CLAUDE_L), LY(CLAUDE_H));
            int v_w = LX(VAISSEAU_L);
            int v_h = LY(VAISSEAU_H);
            masked_stretch_blit(assets->player_sprites, buffer, sx[p->skin_id], sy[p->skin_id], sw[p->skin_id], sh[p->skin_id], LX(0.5) - v_w/2, LY(0.5) - v_h/2, v_w, v_h);

            ecrire_texte(buffer, "Salutations, ",LX(0.188), LY(0.850), makecol(255,255,255), 1.7);
            ecrire_texte(buffer, p->pseudo,LX(0.188)+180, LY(0.850), makecol(255,255,255), 1.7);
            ecrire_texte(buffer, "Je suis l'agent Claude, je serai la pour t'epauler durant cette mission.",
                LX(0.188), LY(0.883), makecol(255,255,255), 1.7);


            if (mouse_b & 1) {
                ATTENDRE_RELACHE();
                while(mouse_b & 1) rest(1);
                audio_stop_speech(audio);  // coupe le speech en cours
                audio_play_speech2(audio); // lance le suivant
                etat_decompte = DECOMPTE2;
            }
        }

        // -------------------------------------------------------
        // DECOMPTE 2
        // -------------------------------------------------------
        else if (etat_decompte == DECOMPTE2) {

            stretch_sprite(buffer, assets->claude, LX(0.025), LY(0.817), LX(CLAUDE_L), LY(CLAUDE_H));
            int v_w = LX(VAISSEAU_L);
            int v_h = LY(VAISSEAU_H);
            masked_stretch_blit(assets->player_sprites, buffer, sx[p->skin_id], sy[p->skin_id], sw[p->skin_id], sh[p->skin_id], LX(0.5) - v_w/2, LY(0.5) - v_h/2, v_w, v_h);

            ecrire_texte(buffer, "La mission si vous l'acceptez,",
                LX(0.188), LY(0.850), makecol(255,255,255), 1.7);
            ecrire_texte(buffer, "est d'eliminer tous les extraterrestres menacant notre planete.",
                LX(0.190), LY(0.883), makecol(255,255,255), 1.7);

            if (mouse_b & 1) {
                ATTENDRE_RELACHE();
                while(mouse_b & 1) rest(1);
                audio_stop_speech(audio);
                audio_play_speech3(audio);
                etat_decompte = DECOMPTE3;
            }
        }

        // -------------------------------------------------------
        // DECOMPTE 3
        // -------------------------------------------------------
        else if (etat_decompte == DECOMPTE3) {

            stretch_sprite(buffer, assets->claude, LX(0.025), LY(0.817), LX(CLAUDE_L), LY(CLAUDE_H));
            int v_w = LX(VAISSEAU_L);
            int v_h = LY(VAISSEAU_H);
            masked_stretch_blit(assets->player_sprites, buffer, sx[p->skin_id], sy[p->skin_id], sw[p->skin_id], sh[p->skin_id], LX(0.5) - v_w/2, LY(0.5) - v_h/2, v_w, v_h);

            ecrire_texte(buffer, "Nous allons ecrire l'histoire, es-tu pret ?",
                LX(0.188), LY(0.850), makecol(255,255,255), 1.7);

            // Bouton PRET  (~19% à 25% en X, ~89% à 92% en Y)
            rectfill(buffer, LX(0.188), LY(0.895), LX(0.248), LY(0.922), makecol(0, 0, 0));
            ecrire_texte(buffer, "PRET", LX(0.190), LY(0.900), makecol(0,255,0), 1.7);

            // Bouton NON   (~37% à 57% en X, ~89% à 92% en Y)
            rectfill(buffer, LX(0.371), LY(0.895), LX(0.571), LY(0.922), makecol(0, 0, 0));
            ecrire_texte(buffer, "Non, j'ai peur", LX(0.375), LY(0.900), makecol(255,0,0), 1.7);

            if (mouse_b & 1) {
                if (mouse_x > LX(0.188) && mouse_x < LX(0.248) &&
                    mouse_y > LY(0.895) && mouse_y < LY(0.922)) {
                    ATTENDRE_RELACHE();
                    while(mouse_b & 1) rest(1);
                    audio_stop_speech(audio);
                    audio_play_speech4(audio);
                    etat_decompte = DECOMPTEY;
                }
                if (mouse_x > LX(0.371) && mouse_x < LX(0.571) &&
                    mouse_y > LY(0.895) && mouse_y < LY(0.922)) {
                    ATTENDRE_RELACHE();
                    while(mouse_b & 1) rest(1);
                    audio_stop_speech(audio);
                    audio_play_speech5(audio);
                    etat_decompte = DECOMPTEN;
                }
            }
        }

        // -------------------------------------------------------
        // DECOMPTE Y (oui, prêt)
        // -------------------------------------------------------
        else if (etat_decompte == DECOMPTEY) {

            stretch_sprite(buffer, assets->claude, LX(0.025), LY(0.817), LX(CLAUDE_L), LY(CLAUDE_H));
            int v_w = LX(VAISSEAU_L);
            int v_h = LY(VAISSEAU_H);
            masked_stretch_blit(assets->player_sprites, buffer, sx[p->skin_id], sy[p->skin_id], sw[p->skin_id], sh[p->skin_id], LX(0.5) - v_w/2, LY(0.5) - v_h/2, v_w, v_h);

            ecrire_texte(buffer, "Parfait, allons ecrire l'histoire !",LX(0.188), LY(0.850), makecol(255,255,255), 1.7);

            if (mouse_b & 1) {
                ATTENDRE_RELACHE();
                audio_stop_speech(audio);
                return JEU;
            }
        }

        // -------------------------------------------------------
        // DECOMPTE N (non, peur)
        // -------------------------------------------------------
        else if (etat_decompte == DECOMPTEN) {

            stretch_sprite(buffer, assets->claude, LX(0.025), LY(0.817), LX(CLAUDE_L), LY(CLAUDE_H));
            int v_w = LX(VAISSEAU_L);
            int v_h = LY(VAISSEAU_H);
            masked_stretch_blit(assets->player_sprites, buffer, sx[p->skin_id], sy[p->skin_id], sw[p->skin_id], sh[p->skin_id], LX(0.5) - v_w/2, LY(0.5) - v_h/2, v_w, v_h);

            ecrire_texte(buffer, "Tant pis... allons quand meme ecrire l'histoire...",
                LX(0.188), LY(0.850), makecol(255,255,255), 1.7);

            if (mouse_b & 1) {
                ATTENDRE_RELACHE();
                audio_stop_speech(audio);
                return JEU;
            }
        }
    return ecran;
}

enum EcranActuel pause_level(enum EcranActuel ecran, BITMAP *buffer) {
    ecrire_centre_texte(buffer, "PAUSE",SCREEN_WIDTH/2, LY(0.133), makecol(100, 150, 255), 10);

    // (~31% à 69% en X, ~33% à 43% en Y)
    rectfill(buffer, LX(0.3125), LY(0.333), LX(0.6875), LY(0.433), makecol(40, 50, 100));
    ecrire_centre_texte(buffer, "Reprendre",SCREEN_WIDTH/2, LY(0.367), makecol(255, 255, 255), 4);


    rectfill(buffer, LX(0.3125), LY(0.467), LX(0.6875), LY(0.567), makecol(30, 30, 50));
    ecrire_centre_texte(buffer, "Sauvegarde",SCREEN_WIDTH/2, LY(0.500), makecol(200, 200, 200), 4);


    rectfill(buffer, LX(0.3125), LY(0.600), LX(0.6875), LY(0.700), makecol(30, 30, 50));
    ecrire_centre_texte(buffer, "Regles",SCREEN_WIDTH/2, LY(0.633), makecol(200, 200, 200), 4);


    rectfill(buffer, LX(0.3125), LY(0.733), LX(0.6875), LY(0.800), makecol(30, 30, 50));
    ecrire_centre_texte(buffer, "Menu principale",SCREEN_WIDTH/2, LY(0.750), makecol(200, 200, 200), 4);


    if (mouse_b & 1) {
        if (mouse_x > LX(0.3125) && mouse_x < LX(0.6875) &&
            mouse_y > LY(0.333)  && mouse_y < LY(0.433)) {
            ATTENDRE_RELACHE();
            return JEU;


            } else if (mouse_x > LX(0.3125) && mouse_x < LX(0.6875) &&
                       mouse_y > LY(0.467)  && mouse_y < LY(0.567)) {
                ATTENDRE_RELACHE();
                return SAUVEGARDE;

                       } else if (mouse_x > LX(0.3125) && mouse_x < LX(0.6875) &&
                                  mouse_y > LY(0.600)  && mouse_y < LY(0.700)) {
                           ATTENDRE_RELACHE();
                           return REGLE;

                                  } else if (mouse_x > LX(0.3125) && mouse_x < LX(0.6875) &&
                                             mouse_y > LY(0.733)  && mouse_y < LY(0.800)) {
                                      ATTENDRE_RELACHE();
                                      return MENU_PRINCIPAL;

                                             }
    }
    return ecran;
}

enum EcranActuel game_over_screen(enum EcranActuel ecran, BITMAP *buffer, Player *p) {
    // Titre "GAME OVER"
    ecrire_centre_texte(buffer, "GAME OVER", SCREEN_WIDTH / 2, LY(0.15), makecol(220, 30, 30), 9);

    // Sous-titre avec le pseudo
    char msg[40];
    snprintf(msg, sizeof(msg), "Vous avez echoue, %s...", p->pseudo);
    ecrire_centre_texte(buffer, msg, SCREEN_WIDTH / 2, LY(0.35), makecol(200, 200, 200), 2);

    // Bouton "Rejouer"
    rectfill(buffer, LX(0.3125), LY(0.50), LX(0.6875), LY(0.58), makecol(40, 80, 40));
    ecrire_centre_texte(buffer, "Rejouer", SCREEN_WIDTH / 2, LY(0.52), makecol(255, 255, 255), 4);

    // Bouton "Menu principal"
    rectfill(buffer, LX(0.3125), LY(0.63), LX(0.6875), LY(0.71), makecol(30, 30, 50));
    ecrire_centre_texte(buffer, "Menu principal", SCREEN_WIDTH / 2, LY(0.65), makecol(200, 200, 200), 4);

    // Bouton "Quitter"
    rectfill(buffer, LX(0.3125), LY(0.76), LX(0.6875), LY(0.81), makecol(50, 20, 20));
    ecrire_centre_texte(buffer, "Quitter", SCREEN_WIDTH / 2, LY(0.775), makecol(200, 200, 200), 4);

    if (mouse_b & 1) {
        // Rejouer → on repart au choix de vaisseau (réinitialise la partie)
        if (mouse_x > LX(0.3125) && mouse_x < LX(0.6875) &&
            mouse_y > LY(0.50)   && mouse_y < LY(0.58)) {
            ATTENDRE_RELACHE();
            return CHOIX;

            // Menu principal
            } else if (mouse_x > LX(0.3125) && mouse_x < LX(0.6875) &&
                       mouse_y > LY(0.63)   && mouse_y < LY(0.71)) {
                ATTENDRE_RELACHE();
                return MENU_PRINCIPAL;

                // Quitter
                       } else if (mouse_x > LX(0.3125) && mouse_x < LX(0.6875) &&
                                  mouse_y > LY(0.76)   && mouse_y < LY(0.81)) {
                           ATTENDRE_RELACHE();
                           return QUITTER;
                                  }
    }
    return ecran;
}