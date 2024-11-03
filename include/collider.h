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
	C_WORLD
}C_Layer;

typedef struct Collider_S
{
	//Entity*			ent; //entity that owns this collider, will break if try to use maybe restructure?
	int*			sectorList; //for spacial division later
	GFC_Primitive	primitive; //primitive struct including type
	GFC_Vector3D	velocity; //how much to move per update
	GFC_Vector3D	position; //current x,y,z position
	//removed scale for now
	GFC_Vector3D	offset; //offset from parent position

	C_Layer			layer;
	Uint8			isTrigger;	//collide or trigger activation




	//behavior
	void (*onTriggerEnter) (struct Collider_S* self, struct Collider_S* other); //called when another collider enters the trigger
//	void (*onTriggerExit) (struct Collider_S* self, struct Collider_S* other); //called when another collider exits the trigger probably don't need this, might for hiding bushes though
}Collider;



/*
 * @brief create new collider for spheres and planes
 * @param type type of primitive to set
 * @param position origin of primitive
 * @param parameter radius or distance 
 * @return self this collider
*/
Collider* sphere_collider_new( GFC_Vector3D position, float radius);

Collider* plane_collider_new(GFC_Vector3D position, float distance);

/*
 * @brief create new collider for boxes and edges
 * @param type type of primitive to set
 * @param position origin of primitive
 * @param position2 2nd position or dimensions
 * @return self this collider
*/
Collider* box_collider_new( GFC_Vector3D position, GFC_Vector3D dimensions);

Collider* edge_collider_new(GFC_Vector3D position, GFC_Vector3D position2);

/*
 * @brief create new collider for triangles
 * @param type type of primitive to set
 * @param vertex1 
 * @param vertex2 
 * @param vertex3 
 * @return self this collider
*/
Collider* triangle_collider_new( GFC_Vector3D vertex1, GFC_Vector3D vertex2, GFC_Vector3D vertex3);
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
Uint8 check_collision(Collider* self, Collider* other);

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





//TODO Raycast
//TODO sphere and box collision





#endif