#ifndef __LEVEL_H_
#define __LEVEL_H_
#include "gfc_audio.h"
#include "gfc_config.h"
#include "gf3d_mesh.h" //maybe bad
#include "gf3d_obj_load.h"

#include "environment.h"
#include "player.h"
#include "prey.h"
#include "predator.h"

typedef struct Level_S
{	
	ObjData*		level_obj;
	Entity*			predator_list; //store entity indices instead prolly
	Entity*			prey_list;
	int				predator_total;
	int				prey_total;
	Uint8			complete;
	Uint8			failed;
	
	//current level name

}Level;

typedef struct Game_Manager_S
{
	Entity*			player;	
	GFC_Sound*		bgm; //background music
	GFC_Sound*		menu_sound; //menu transition noise
	Level			current_level;
	char*			current_level_file; //GFC_TextLine	name;
	Uint8			pause;
	Uint8			main_menu;
	

}Game_Manager;

void set_player(Game_Manager* gm, Entity* player);
Entity* get_player(Game_Manager* gm);

Game_Manager* new_game_manager();

void exit_to_menu(Game_Manager* game_manager);
void load_from_menu(Game_Manager* game_manager, const char* filename);
void draw_menu();

Level load_level_config_from_file(Game_Manager* game_manager, const char* filename);
//Level get_level_from_config(SJson* config);
Level change_level(Game_Manager* game_manager, const char* filename); //take level name? config folder?

void level_complete(Game_Manager* game_manager);
void level_fail(Game_Manager* game_manager);

void game_update(Game_Manager* self);

void game_manager_free(Game_Manager* game_manager);
#endif