#ifndef __PLAYER_H_
#define __PLAYER_H_

#include "entity.h"

typedef struct Player_Data_S
{
	int				health;
	int				prey_eaten;

}Player_Data;

/**
* @brief  maka da playa data
* @return player data pointer or NULL on failure
*/
Player_Data* player_data_new();

/**
* @brief  maka da playa
* @return player ent, or NULL on failure
*/
Entity* player_new();


/**
* @brief  lock cam to offset of player position and look at player
* @param self entity to follow
* @param offset Distance to set camera back
*/
void cam_follow_player(Entity* self, float offset);

void player_walk_forward(Entity* self, float magnitude);
void player_walk_right(Entity* self, float magnitude);

void player_move_up(Entity* self, float magnitude);

void player_think(Entity* self);

void player_update(Entity* self);

void player_free(Entity* self);



#endif