#ifndef __PREY_H__
#define __PREY_H__

#include "entity.h"
/*
typedef struct Prey_Data_S
{

	Model* model; // entity's model if it has one
	Collider* collider;
	GFC_Vector3D	position;
	GFC_Vector3D	rotation;
	GFC_Vector3D	scale;
	GFC_TextLine	name;
	Uint8			_inuse;	//flag for memory management




	                          //entity custom data beyond basics
}Prey_Data;

*/

/*
 * @brief creates prey entity
*/
Entity* prey_new();

void prey_think(Entity* self);
void prey_update(Entity* self);
void prey_free(Entity* self);
void prey_eaten(Collider* self, Collider* other);
void prey_chillin(Collider* self, Collider* other);
#endif