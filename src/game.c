#include <SDL.h>            

#include "simple_json.h"
#include "simple_logger.h"

#include "gfc_input.h"
#include "gfc_config_def.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"
#include "gfc_audio.h"
#include "gfc_string.h"
#include "gfc_actions.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"
#include "gf2d_actor.h"
#include "gf2d_mouse.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_draw.h"

//#include "gf3d_obj_load.h" in level now

//#include "entity.h" in player.h
//#include "player.h" in level.h
#include "prey.h"
#include "environment.h"
#include "level.h"

//#include "physics.h" included in entity now

extern int __DEBUG;

static int _done = 0;
static Uint32 frame_delay = 33;
static float fps = 0;

void parse_arguments(int argc,char *argv[]);
void game_frame_delay();

void exitGame()
{
    _done = 1;
}

void draw_origin()
{
    gf3d_draw_edge_3d(
        gfc_edge3d_from_vectors(gfc_vector3d(-100,0,0),gfc_vector3d(100,0,0)),
        gfc_vector3d(0,0,0),gfc_vector3d(0,0,0),gfc_vector3d(1,1,1),0.1,gfc_color(1,0,0,1));
    gf3d_draw_edge_3d(
        gfc_edge3d_from_vectors(gfc_vector3d(0,-100,0),gfc_vector3d(0,100,0)),
        gfc_vector3d(0,0,0),gfc_vector3d(0,0,0),gfc_vector3d(1,1,1),0.1,gfc_color(0,1,0,1));
    gf3d_draw_edge_3d(
        gfc_edge3d_from_vectors(gfc_vector3d(0,0,-100),gfc_vector3d(0,0,100)),
        gfc_vector3d(0,0,0),gfc_vector3d(0,0,0),gfc_vector3d(1,1,1),0.1,gfc_color(0,0,1,1));
}


int main(int argc,char *argv[])
{
    //initializtion    
    parse_arguments(argc,argv);
    init_logger("gf3d.log",0);
    slog("gf3d begin");
    //gfc init
    gfc_input_init("config/input.cfg");
    gfc_config_def_init();
    gfc_action_init(1024);
    gfc_audio_init(10, 4, 1, 1, 0, 0);
    //gf3d init
    gf3d_vgraphics_init("config/setup.cfg");
    gf3d_materials_init();
    gf2d_font_init("config/font.cfg");
    gf2d_actor_init(1000);
    gf3d_draw_init();//3D
    gf2d_draw_manager_init(1000);//2D

    //new entity stuff, always do after prereq inits
    entity_system_init(1024);
    
    //game init
    srand(SDL_GetTicks());
    slog_sync();

    //game setup
    //local variables
    
    //Entity* player;
    //Entity* prey, *prey2, * prey3, * prey4;
    //Entity* ground;
    Model* sky;
    GFC_Matrix4 skyMat;
    Game_Manager* gm = new_game_manager();
    Uint8 player_colliding;
    Texture* og = gf3d_texture_load("models/player/player.png");
    //ground = environment_new();
    //player = player_new();
    //set_player(gm, player);

    gf2d_mouse_load("actors/mouse.actor");
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix4_identity(skyMat);

    
    
    /*prey = prey_new();
    prey2 = prey_new();
    prey2->collider->isTrigger = 0;
    prey2->collider->position.y = -2;
    prey3 = prey_new();
    prey4 = prey_new();
    prey3->collider->position.y = 3;
    prey4->collider->position.y = 7;
    prey4->collider->layer = C_PREDATOR;
    prey4->collider->isTrigger = 0;
    */
    //ground = environment_new();
    //ground->position = gfc_vector3d(0, 0, -5);
        
        //camera
    gf3d_camera_set_scale(gfc_vector3d(1,1,1));
    gf3d_camera_set_position(gfc_vector3d(0,0,5));
    gf3d_camera_look_at(gfc_vector3d(0,0,0),NULL);
    gf3d_camera_set_move_step(0.2);
    gf3d_camera_set_rotate_step(0.05);
    
    gf3d_camera_enable_free_look(0);
    gf3d_camera_set_auto_pan(0);

    //lock mouse to game
    SDL_Window* sdl_win = gf3d_vgraphics_get_SDL_Window();
    GFC_Vector2D dims;
    dims = gf3d_vgraphics_get_SDL_window_size();
    
    //SDL_GetWindowMinimumSize
    SDL_SetRelativeMouseMode(SDL_TRUE); //this just doesnt work
    //SDL_CaptureMouse
    //SDL_SetWindowGrab(gf3d_vgraphics_get_SDL_Window, SDL_TRUE);
        
    //windows

    //slog("vertex count %d", currobj->vertex_count);
    //slog("normal count %d", currobj->normal_count);
    gm->current_level = load_level_config_from_file(gm, "levels/level1.cfg");
    //slog("prey count %d", uno.prey_total);
    //slog
    //set_player(gm, player);
    if (gm && gm->bgm)
        gfc_sound_play(gm->bgm, 100, 50, -1, -1);
    // main game loop    
    while(!_done)
    {
        //this is god awful, fix edge trtansition stutter by adding extra rotation to cam_target at loop point
        GFC_Vector2D mouse_pos = gf2d_mouse_get_position();
       // /*
        if (mouse_pos.x >= dims.x-1) SDL_WarpMouseInWindow(sdl_win, 1, mouse_pos.y);
        if (mouse_pos.x <= 0) SDL_WarpMouseInWindow(sdl_win, dims.x-2, mouse_pos.y);
        if (mouse_pos.y >= dims.y-1) SDL_WarpMouseInWindow(sdl_win, mouse_pos.x, 1);
        if (mouse_pos.y <= 0) SDL_WarpMouseInWindow(sdl_win, mouse_pos.x, dims.y-2);

       // */
       // slog("w: %f, h: %f, x: %f, y: %f", dims.x, dims.y, mouse_pos.x, mouse_pos.y);

        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();
        if (gm) {
            game_update(gm);
        }
        if (gm && !gm->main_menu) {
            if (!gm->pause) {
                //ent stuff here after input before draws? 
                entity_think_all();
                //check mesh here
                if (gm->player->capsule) {
                    if (capsule_to_mesh_collision(gm->player->collider, gm->player->capsule, gm->current_level.level_obj)) {
                        gm->player->collider->gravity = 0;
                        gm->player->model->texture = gm->current_level.level_model->texture;
                    }
                    
                        
                    else {
                        gm->player->collider->gravity = 0.002;
                        //gm->player->model->texture = og;
                    }
                                                 

                }
                    

                entity_update_all();

                //camera work should prob go last - Josh
                //camera updates
                gf3d_camera_controls_update();

                gf3d_camera_update_view();
                gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());
            }
        }
        gf3d_vgraphics_render_start();
            if (!gm->main_menu) {
                //3D draws      
                gf3d_model_draw_sky(sky, skyMat, GFC_COLOR_WHITE);
                //always do ui draws after world draws, which are after backgroun draws (skybox)
                entity_draw_all();

                /*    gf3d_model_draw(
                    dino,
                    dinoMat,
                    GFC_COLOR_WHITE,
                    0);
                */

                //draw_origin();
                //2D draws
                gf2d_mouse_draw();
            }
            if (gm->main_menu) {
                draw_menu();
                
            }
            else {
                draw_hud(gm->player);
            }

                gf2d_font_draw_line_tag("ALT+F4 to exit",FT_H1,GFC_COLOR_WHITE, gfc_vector2d(10,10));
                //make ui function suite and file later probably  
                // 
                //hud       
                
                //slog("about to do UI");
                
        gf3d_vgraphics_render_end();
        if (gfc_input_command_down("exit"))_done = 1; // exit condition
        game_frame_delay();
    }    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    //entity_free(player); //what was I even doing here lol, this caused hang on exit
    //mesh_collider_free(mc); 
    game_manager_free(gm);
    free(gm); //make this atexit function later
   // Mix_CloseAudio();
    slog("gf3d program end");
    //SDL_SetRelativeMouseMode(SDL_FALSE); //probably dont need this but
    //SDL_SetWindowGrab(gf3d_vgraphics_get_SDL_Window, SDL_FALSE);
    exit(0); //calls all atexit() functions automatically
    slog_sync();
    return 0;
}

void parse_arguments(int argc,char *argv[])
{
    int a;

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }    
}

void game_frame_delay()
{
    Uint32 diff;
    static Uint32 now;
    static Uint32 then;
    then = now;
    slog_sync();// make sure logs get written when we have time to write it
    now = SDL_GetTicks();
    diff = (now - then);
    if (diff < frame_delay)
    {
        SDL_Delay(frame_delay - diff);
    }
    fps = 1000.0/MAX(SDL_GetTicks() - then,0.001);
//     slog("fps: %f",fps);
}
/*eol@eof*/
