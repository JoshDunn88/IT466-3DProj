#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gfc_primitives.h"
#include "gf3d_mesh.h"
#include "gf3d_obj_load.h"

//collision layers

typedef struct Capsule_S
{
	GFC_Vector3D	position; //center position 
	GFC_Vector3D	length;   //length of cylindrical portion of capsule


	float			radius; // radius from center axis

}Capsule;

typedef struct Mesh_Collider_S
{
	Face* triangle_verts; //triangle prims
	Face* triangle_normals; //triangle normals
	Uint32	tri_count;   //length of cylindrical portion of capsule


}Mesh_Collider;



Mesh_Collider* mesh_collider_new(ObjData* obj);
Uint8 sphere_to_mesh_collision(GFC_Sphere sphere, ObjData* obj);
void mesh_collider_free(Mesh_Collider* mc);

#endif