#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gfc_primitives.h"
#include "gf3d_mesh.h"
#include "gf3d_obj_load.h"

#include "collider.h"

//collision layers

typedef struct Capsule_S
{
	GFC_Vector3D	position; //center position 
	GFC_Vector3D	length;   //length of cylindrical portion of capsule


	float			radius; // radius from center axis

}Capsule;

typedef struct Mesh_Collider_S
{
	Face* triangle_verts; //indices of triangle verts
	Face* triangle_normals; //indices of triangle normals
	Uint32	tri_count;   //number of triangles


}Mesh_Collider;

GFC_Vector3D gfc_vector3d_scaled(GFC_Vector3D v, float scalar);
Uint8 gfc_vector3d_equal(GFC_Vector3D a, GFC_Vector3D b);

//sphere v triangle, capsule and mesh stuff from https://wickedengine.net/2020/04/capsule-collision-detection/

Mesh_Collider* mesh_collider_new(ObjData* obj);
GFC_Vector3D ClosestPointOnLineSegment(GFC_Vector3D a, GFC_Vector3D b, GFC_Vector3D point);
Uint8 sphere_to_triangle_collision(GFC_Sphere s, GFC_Triangle3D t, GFC_Vector3D* pen_norm, float* pen_depth);
Uint8 sphere_to_mesh_collision(Collider* col, ObjData* obj);
void sphere_to_triangle_resolution(Collider* col, GFC_Triangle3D t, GFC_Vector3D normal, float depth);
void mesh_collider_free(Mesh_Collider* mc);

#endif