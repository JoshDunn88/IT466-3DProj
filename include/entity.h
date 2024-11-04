#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_text.h"
#include "gfc_vector.h"
#include "gf3d_model.h"
#include "gf3d_draw.h"
#include "collider.h"

typedef struct Entity_S
{
	
	Model*			model; // entity's model if it has one
	Collider*		collider;
	GFC_Vector3D	position;
	GFC_Vector3D	rotation;
	GFC_Vector3D	scale;
	GFC_TextLine	name;
	Uint8			_inuse;	//flag for memory management
	Uint8			alive;	//flag for life
	
	
	 

	//behavior
	void (*think) (struct Entity_S* self); //called every frame for entity to decide things
	void (*update) (struct Entity_S* self); //called every frame to update its state
	int (*draw) (struct Entity_S* self); //custom drawing code, if -1 skip
	void (*free) (void* medata); //called when entity is cleaned up to clean custom data
	void* data;                            //entity custom data beyond basics
}Entity;

/*
 * @brief initializes entity management system and queues cleanup on exit
 * @param maxEnts how many entities can exist at the same time
*/
void entity_system_init(Uint32 maxEnts);

void entity_system_close();

/*
 * @brief clean up active entities
 * @param ignore do not clean this up
*/
void entity_clear_all(Entity* ignore);

void check_collisions(Collider* self);


/*
 * @brief think for all entities
 *  

*/
void entity_think_all();

/*
 * @brief get entity by its collider
 *

*/
Entity* entity_get_by_collider(Collider* self);
/*
 * @brief update for all entities
 *

*/
void entity_update_all();

/*
 * @brief draw all active entities

*/
void entity_draw_all();

/*
 * @brief get a blank entity for use
 * @return NULL on failure (no more room), or a pointer to initialized entity

*/
Entity* entity_new();

/*
 * @brief clean up  entity and free its spot
 * @param self the entity to free

*/
void entity_free(Entity* self);

void entity_draw(Entity* self);

void entity_think(Entity* self);

void entity_update(Entity* self);

#endif