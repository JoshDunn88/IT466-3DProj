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

#include "entity.h"
#include "player.h"
#include "prey.h"
#include "environment.h"


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
    
    //local variables
    Entity* player;
    Player_Data* dat;
    Entity* prey, *prey2, * prey3, * prey4;
    Entity* ground;
    Model *sky,*dino;
    GFC_Matrix4 skyMat,dinoMat;
    //GFC_String* health, *food;
    char* healthVal, *foodVal, *speed;
    //initializtion    
    parse_arguments(argc,argv);
    init_logger("gf3d.log",0);
    slog("gf3d begin");
    //gfc init
    gfc_input_init("config/input.cfg");
    gfc_config_def_init();
    gfc_action_init(1024);
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
    gf2d_mouse_load("actors/mouse.actor");
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix4_identity(skyMat);

    
    player = player_new();
    dat = (struct Player_Data*)(player->data);
    prey = prey_new();
    prey2 = prey_new();
    prey2->collider->isTrigger = 0;
    prey2->collider->position.y = -2;
    prey3 = prey_new();
    prey4 = prey_new();
    prey3->collider->position.y = 3;
    prey4->collider->position.y = 7;

    ground = environment_new();
    //health = gfc_string("Health: ");
    //food = gfc_string("Food: ");
    healthVal = "";
    foodVal = "";
    speed = "";
        //camera
    gf3d_camera_set_scale(gfc_vector3d(1,1,1));
    gf3d_camera_set_position(gfc_vector3d(0,0,5));
    gf3d_camera_look_at(gfc_vector3d(0,0,0),NULL);
    gf3d_camera_set_move_step(0.2);
    gf3d_camera_set_rotate_step(0.05);
    
    gf3d_camera_enable_free_look(0);
    gf3d_camera_set_auto_pan(0);
    //windows

    // main game loop    
    while(!_done)
    {
        gfc_input_update();
        gf2d_mouse_update();
        gf2d_font_update();
        

        //ent stuff here after input before draws? 
        entity_think_all();
        entity_update_all();

        //make function later
        sprintf(&healthVal, "%i", dat->health);
        sprintf(&foodVal, "%i", dat->prey_eaten);
        sprintf(&speed, "%f", gfc_vector3d_magnitude(player->collider->velocity));
        //gfc_string_append(health, healthVal);
        //gfc_string_append(food, foodVal);

        //camera work should prob go last - Josh
        //camera updates
        gf3d_camera_controls_update();
        cam_follow_player(player, -10);
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());


        gf3d_vgraphics_render_start();

            //3D draws
        
                gf3d_model_draw_sky(sky,skyMat,GFC_COLOR_WHITE);

                //always do ui draws after world draws, which are after backgroun draws (skybox)
                entity_draw_all();
              

            /*    gf3d_model_draw(
                    dino,
                    dinoMat,
                    GFC_COLOR_WHITE,
                    0);
                */

                
                draw_origin();
            //2D draws
                gf2d_mouse_draw();
                gf2d_font_draw_line_tag("ALT+F4 to exit",FT_H1,GFC_COLOR_WHITE, gfc_vector2d(10,10));
                //make ui function suite and file later probably         
                    
                //slog("about to do UI");
                gf2d_font_draw_line_tag(&healthVal, FT_H1, GFC_COLOR_GREEN, gfc_vector2d(300, 10));
                gf2d_font_draw_line_tag(&foodVal, FT_H1, GFC_COLOR_YELLOW, gfc_vector2d(600, 10));
                gf2d_font_draw_line_tag(&speed, FT_H1, GFC_COLOR_BLUE, gfc_vector2d(900, 10));
        gf3d_vgraphics_render_end();
        if (gfc_input_command_down("exit"))_done = 1; // exit condition
        game_frame_delay();
    }    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    entity_free(player); //likely unneccesary, but is extra clean, like washing below the knee
    slog("gf3d program end");
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
