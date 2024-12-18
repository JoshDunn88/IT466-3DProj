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
void gfc_vector3d_angle_vectors3(GFC_Vector3D angles, GFC_Vector3D* forward, GFC_Vector3D* right, GFC_Vector3D* up);


Capsule* capsule_new(GFC_Vector3D position, GFC_Vector3D rotation, float length, float radius);
GFC_Edge3D get_capsule_endpoints(Capsule* capsule);

//sphere v triangle, capsule stuff from https://wickedengine.net/2020/04/capsule-collision-detection/
GFC_Vector3D ClosestPointOnCapsule(Capsule* capsule, GFC_Triangle3D tri);
GFC_Vector3D ClosestPointOnLineSegment(GFC_Vector3D a, GFC_Vector3D b, GFC_Vector3D point);
Uint8 sphere_to_triangle_collision(GFC_Sphere s, GFC_Triangle3D t, GFC_Vector3D* pen_norm, float* pen_depth);
void sphere_to_triangle_resolution(Collider* col, GFC_Triangle3D t, GFC_Vector3D normal, float depth);
//change return to be corrected position and velocity

Uint8 sphere_to_mesh_collision(Collider* col, ObjData* obj);
Uint8 capsule_to_mesh_collision(Capsule* cap, ObjData* obj);


#endif