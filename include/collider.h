#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "entity.h"
#include "gfc_primitives.h"



typedef struct Collider_S
{
	Entity*			ent; //entity that owns this collider
	int*			sectorList; //for spacial division later
	GFC_Primitive	primitive; //primitive struct including type
	GFC_Vector3D	offset; //offset from parent position
	GFC_Vector3D	scale; //scale relative to parent position
	Uint8			isTrigger;	//collide or trigger activation




	//behavior
	void (*onTriggerEnter) (struct Collider_S* self, struct Collider_S* other); //called when another collider enters the trigger
	void (*onTriggerExit) (struct Collider_S* self, struct Collider_S* other); //called when another collider exits the trigger
}Collider;















#endif