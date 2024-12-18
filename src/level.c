#include "simple_logger.h"
#include "level.h"

Uint8 P, ESC;

void set_player(Game_Manager* gm, Entity* player) {
    if (!gm || !player) return;
        gm->player = player;
}

Entity* get_player(Game_Manager* gm) {
    return gm->player;
}
Game_Manager* new_game_manager() {
    Game_Manager* gm = malloc(sizeof(Game_Manager));
    if (!gm) {
        slog("could not allocate game manager");
        return NULL;
    }
    gm->player = NULL;
    gm->menu_sound = gfc_sound_load("sounds/776021__charliesmiler__bass-wobble-link-to-wav-packs-below.wav", 1, 1);
    if (!gm->menu_sound)
        slog("failed to set sound");
    gm->bgm = gfc_sound_load("music/617285__hermanruzh__shostakovich-second-waltz-on-guitar.wav", 1, 1);
    if (!gm->bgm)
        slog("failed to set bgm");
    //gm level = main menu level
    gm->current_level_file = NULL;;
    gm->pause = false;
    gm->main_menu = true;
    return gm;
}
void exit_to_menu(Game_Manager* game_manager) {}
void load_from_menu(Game_Manager* game_manager, const char* filename) {
    if (!game_manager || !filename)
        return;
    //do level stuff

}
Level get_level_from_file(const char* filename) {}
void change_level(Game_Manager* game_manager, const char* filename) {}
void level_complete(Game_Manager* game_manager) {}
void level_fail(Game_Manager* game_manager) {}
void game_update(Game_Manager* self) {
    const Uint8* keys;
    
    if (!self) return;
    
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

    if (keys[SDL_SCANCODE_P])
    {
        if(!P){
            if (self->pause) {
                self->pause = false;
                slog("paused");
            }
           
            else if (!self->main_menu) {
               self->pause = true;
               slog("unpaused");
            }
            P = true;
        }
    }
    else if (P) {
        P = false;
    }

    if (keys[SDL_SCANCODE_ESCAPE])
    {
        if (!ESC) {
            if (self->main_menu) {
                self->main_menu = false;
                if(self->player)
                    reset_player(self->player);
                if (self->menu_sound) {
                    gfc_sound_play(self->menu_sound, 0, 100, -1, -1);
                    slog("played menu sound %s", self->menu_sound->filepath);
                    }
            }
           //load_from_menu(self, "placeholder");
            else {
                self->main_menu = true; //exit_to_menu(self);
                if (self->bgm) {
                    gfc_sound_play(self->bgm, 0, 1, -1, -1);
                    slog("played menu bgm %s", self->bgm->filepath);
                }
                   
            }
            ESC = true;
        }
    }
    else if (ESC){
        ESC = false;
    }

}

void draw_menu() {
    gf2d_font_draw_line_tag("CHAMELEON SIMLATOR 2025", FT_Large, GFC_COLOR_GREEN, gfc_vector2d(400, 300));
    gf2d_font_draw_line_tag("Press Esc to start the game", FT_Normal, GFC_COLOR_WHITE, gfc_vector2d(450, 400));
}

void game_manager_free(Game_Manager* game_manager) {
    if (!game_manager) return;
    if (game_manager->bgm) {
        gfc_sound_free(game_manager->bgm);
        free(game_manager->bgm);
    }
    if (game_manager->menu_sound) {
        gfc_sound_free(game_manager->menu_sound);
        free(game_manager->menu_sound);
    }

}