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
	GFC_Vector3D	rotation; //rotation of capsule
	float			length;   //length of center axis of capsule
	float			radius; // radius from center axis

}Capsule;


GFC_Vector3D gfc_vector3d_scaled(GFC_Vector3D v, float scalar);
Uint8 gfc_vector3d_equal(GFC_Vector3D a, GFC_Vector3D b);



Capsule* capsule_new(GFC_Vector3D position, GFC_Vector3D rotation, float length, float radius);
GFC_Edge3D get_capsule_endpoints(Capsule* capsule);

//sphere v triangle, capsule stuff from https://wickedengine.net/2020/04/capsule-collision-detection/
GFC_Vector3D ClosestPointOnCapsule(GFC_Vector3D a, GFC_Vector3D b, GFC_Triangle3D tri);
GFC_Vector3D ClosestPointOnLineSegment(GFC_Vector3D a, GFC_Vector3D b, GFC_Vector3D point);
Uint8 sphere_to_triangle_collision(GFC_Sphere s, GFC_Triangle3D t, GFC_Vector3D* pen_norm, float* pen_depth);
void sphere_to_triangle_resolution(Collider* col, GFC_Triangle3D t, GFC_Vector3D normal, float depth);

Uint8 sphere_to_mesh_collision(Collider* col, ObjData* obj);



#endif