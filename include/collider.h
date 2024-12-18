#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "gfc_types.h"
#include "gfc_vector.h"
//#include "entity.h"
#include "gfc_primitives.h"

typedef enum
{
	C_DEFAULT,
	C_PLAYER,
	C_PREY,
	C_PREDATOR,
	C_WORLD
}C_Layer;

typedef struct Collider_S
{
	//behavior
	void (*onTriggerEnter) (struct Entity* self, struct Entity* other); //called when another collider enters the trigger
	void (*onTriggerExit) (struct Collider_S* self, struct Collider_S* other); //called when another collider exits the trigger probably don't need this, might for hiding bushes though
	void (*whileTrigger) (struct Collider_S* self, struct Collider_S* other); //called while in trigger after entry before exit
	//data
	//Entit*			ent; //entity that owns this collider, will break if try to use maybe restructure?
	//int*			sectorList; //for spacial division later
	GFC_Primitive	primitive; //primitive struct including type
	GFC_Vector3D	velocity; //how much to move per update
	GFC_Vector3D	position; //center position 
	
	GFC_Vector3D	scale; // hmmm
	GFC_Vector3D	offset; //offset from parent position not yet implemented, do this so player collider is mouth trigger?
	float			gravity;

	C_Layer			layer;
	Uint8			isTrigger;	//collide or trigger activation
	Uint8			triggerActive;	//is something in trigger
	



	
}Collider;

Collider* collider_setup(GFC_Primitive prim);

/*
 * @brief create new collider for spheres and planes
 * @param type type of primitive to set
 * @param position origin of primitive
 * @param parameter radius or distance 
 * @return self this collider
*/
Collider* sphere_collider_new( GFC_Vector3D position, float radius);


/*
 * @brief create new collider for boxes 
 * @param type type of primitive to set
 * @param position origin of primitive
 * @param position2 2nd position or dimensions
 * @return self this collider
*/
Collider* box_collider_new( GFC_Vector3D position, GFC_Vector3D dimensions);

/*
 * @brief free collider
 * @param self this collider
*/
void collider_free(Collider* self);

/*
 * @brief make collider trigger or not
 * @param self this collider
 * @param toBeTrigger trigger if 1 non-trigger if 0
*/
void set_as_trigger(Collider* self, Uint8 toBeTrigger);

/*
 * @brief check collisions for this object and all others nearby
 * @param self this collider
*/
//void check_collisions(Collider* self); gonna do this in entity instead

/*
 * @brief test if this object colliding with other
 * @param self this collider
 * @param other other collider
 * @return 1 if yes, 0 if no
*/
Uint8 check_collision(Collider* self, Collider* other, GFC_Vector3D self_velocity, GFC_Vector3D other_velocity);

/*
 * @brief do collision effect for this and other
 * @param self this collider
 * @param other other collider
*/
void do_collision(Collider* self, Collider* other);

/*
 * @brief apply movement to collider
 * @param self this collider
 * @param other other collider
*/
void collider_update(Collider* self);

/*
 * @brief check box overlap with offset parameters
 * @param a box 1
 * @param b box 2
 * @param a_delta box 1 delta
 * @param b_delta box 2 delta
*/
Uint8 predictive_box_overlap(GFC_Box a, GFC_Box b, GFC_Vector3D a_delta, GFC_Vector3D b_delta);


//TODO Raycast
//TODO sphere and box collision





#endif