#ifndef __PLAYER_H_
#define __PLAYER_H_

#include "entity.h"
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



#endif