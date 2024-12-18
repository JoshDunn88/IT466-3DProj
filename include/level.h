#ifndef __LEVEL_H_
#define __LEVEL_H_

#include "player.h"

typedef struct Level_S
{
	Entity*			predator_list;
	Entity*			prey_list;
	int				predator_total;
	int				prey_total;
	//current level name

}Level;

typedef struct Game_Manager_S
{
	Entity*			player;	
	Level			current_level;
	char*			current_level_file;
	Uint8			pause;
	Uint8			main_menu;
	

}Game_Manager;

void set_player(Game_Manager* gm, Entity* player);
Entity* get_player(Game_Manager* gm);
Game_Manager* new_game_manager();
void exit_to_menu(Game_Manager* game_manager);
void load_from_menu(Game_Manager* game_manager, const char* filename);
Level get_level_from_file(const char* filename);
void change_level(Game_Manager* game_manager, const char* filename); //take level name? config folder?
void level_complete(Game_Manager* game_manager);
void level_fail(Game_Manager* game_manager);
void game_update(Game_Manager* self);

#endif