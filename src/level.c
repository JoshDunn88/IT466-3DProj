#include "simple_logger.h"
#include "level.h"

Uint8 P, ESC, ONE, TWO;

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
    //config
}
Level load_level_config_from_file(Game_Manager* game_manager, const char* filename) {
    SJson* config = sj_load(filename);
    SJson* current = NULL;
    SJson* item, *preylist, *predatorlist;
    Entity* prey, *predator, *environment, *player;
    const char* meshfile;
    Model* world;
    //SJson* current = NULL;
    Level level = { 0 };
    int i;

    //start level stuff like entity init etc? or separate function?

    //player stuff
    player = player_new();
    set_player(game_manager, player);


    if (!config) {
        slog("failed to load level from ");
            return level;
    }
    current = sj_object_get_value(config, "world");
    if (current) {
        meshfile = sj_object_get_value_as_string(current, "mesh");
        environment = environment_new();
        environment->position = gfc_vector3d(0, 0, 0);
        environment->scale = gfc_vector3d(1, 1, 1);
        environment->rotation = gfc_vector3d(0, 0, 0);
        world = gf3d_model_load_full(meshfile, sj_object_get_value_as_string(current, "texture"));
        if (world) {
            environment->model = world;
            Mesh* currmesh = (struct Mesh*)(world->mesh_list->elements[0].data);
            MeshPrimitive* currprim = (struct MeshPrimitive*)(currmesh->primitives->elements[0].data);
            level.level_obj = currprim->objData;
        }
        else
            slog("could not load world");
    }
    else
        slog("did not find world");

    current = sj_object_get_value(config, "player");
    if (current) {
        sj_value_as_vector3d(sj_object_get_value(current, "spawn"), &player->collider->position); //fix this with spawn function
    }
    else
        slog("did not find player data");
    
    current = sj_object_get_value(config, "predator");
    if (current)
    {
        slog("about to get count");
        predatorlist = sj_object_get_value(current, "predatorlist");
        level.predator_total = sj_array_get_count(predatorlist);
        if (level.predator_total == 0)
        {
            slog("predator config has no predator");
        }
        else
        {
            for (i = 0; i < level.predator_total; i++)
            {
                item = sj_array_get_nth(predatorlist, i);
                if (!item)continue;
                predator = prey_new();
                predator->collider->layer = C_PREDATOR;
                predator->collider->isTrigger = 0;
                 sj_value_as_vector3d(sj_object_get_value(item, "spawn"), &predator->collider->position); //fix this with spawn function later
                 sj_value_as_vector3d(sj_object_get_value(item, "spawn"), &predator->position);
                 collider_update(predator->collider);
                 slog("spawned new predator at %f, %f, %f ", predator->position.x, predator->position.y, predator->position.z);
            }
        }
    }
    else
        slog("failed to load predatorlist");

    current = sj_object_get_value(config, "prey");
    if (current)
    {
        slog("about to get count");
        preylist = sj_object_get_value(current, "preylist");
        level.prey_total = sj_array_get_count(preylist);
        if (level.prey_total == 0)
        {
            slog("prey config has no prey");
        }
        else
        {
            for (i = 0; i < level.prey_total; i++)
            {
                item = sj_array_get_nth(preylist, i);
                if (!item)continue;
                prey = prey_new();
                sj_value_as_vector3d(sj_object_get_value(item, "spawn"), &prey->collider->position); //fix this with spawn function later
                sj_value_as_vector3d(sj_object_get_value(item, "spawn"), &prey->position);
                collider_update(prey->collider);
                slog("spawned new prey at %f, %f, %f ", prey->position.x, prey->position.y, prey->position.z);
            }
        }
    }
    else
        slog("failed to load preylist");

    slog("finna free");
    sj_free(config);
    //sj_free(current);
    //slog("finna free 2");
    //free(config); 
    slog("returning level");
    return level;
}




Level change_level(Game_Manager* game_manager, const char* filename) {
    entity_clear_all(NULL);
    game_manager->current_level;
    game_manager->pause = false;
    game_manager->main_menu = true;
    return load_level_config_from_file(game_manager, filename);    
}

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
    if (keys[SDL_SCANCODE_1])
    {
        if (!ONE) {
            self->current_level = change_level(self, "levels/level1.cfg");     
            ONE = true;
        }
    }
    else if (ONE) {
        ONE = false;
    }
    if (keys[SDL_SCANCODE_2])
    {
        if (!TWO) {
            self->current_level = change_level(self, "levels/level2.cfg");
            TWO = true;
        }
    }
    else if (TWO) {
        TWO = false;
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
        //free(game_manager->bgm);
    }
    if (game_manager->menu_sound) {
        gfc_sound_free(game_manager->menu_sound);
        //free(game_manager->menu_sound);
    }

}