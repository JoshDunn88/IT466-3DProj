#include "simple_logger.h"
#include "level.h"

Uint8 P, ESC;

void set_player(Game_Manager* gm, Entity* player) {
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
    //gm level = main menu level
    gm->current_level_file = NULL;;
    gm->pause = false;
    gm->main_menu = true;
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
           
            else {
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
            }
           //load_from_menu(self, "placeholder");
            else
                self->main_menu = true; //exit_to_menu(self);
            ESC = true;
        }
    }
    else if (ESC){
        ESC = false;
    }

}

void draw_menu() {
    gf2d_font_draw_line_tag("Press Esc to start the game", FT_H1, GFC_COLOR_GREEN, gfc_vector2d(600, 600));
}