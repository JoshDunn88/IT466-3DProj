#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_text.h"
#include "gfc_vector.h"
#include "gf3d_model.h"

typedef struct Entity_S
{
	Uint8			_inuse;	//flag for memory management
	GFC_TextLine	name;
	GFC_Vector3D	position;
	GFC_Vector3D	rotation;
	GFC_Vector3D	scale; 
	Model			*model;

	//behavior
	void (*think) (struct Entity_S* self); //called every frame for entity to decide things
	void (*update) (struct Entity_S* self); //called every frame to update its state
	int (*draw) (struct Entity_S* self); //custom drawing code, if -1 skip
	void (*free) (struct Entity_S* self); //called when entity is cleaned up to clean custom data
	void			*data //entity custom data beyond basics
}Entity;

/*
 * @brief allocated a blank entity for use
 * @param maxEnts how many entities can exist at the same time
*/
void entity_system_init(Uint32 maxEnts);

/*
 * @brief allocated a blank entity for use
 * @param maxEnts how many entities can exist at the same time
*/
void entity_system_init(Uint32 maxEnts);
/*
 * @brief allocated a blank entity for use
 * @return NULL on failure or a pointer to initialized entity

*/
Entity* entity_new();

/*
 * @brief allocated a blank entity for use
 * @return NULL on failure or a pointer to initialized entity

*/
Entity* entity_free();

#endif