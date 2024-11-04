
#include "simple_logger.h"
#include "gfc_matrix.h"
#include "collider.h"


Collider* collider_setup(GFC_Primitive prim) {
	Collider* self = malloc(sizeof(Collider));
	if (!self) {
		slog("could not allocate collider");
		return NULL;
	}

	self->primitive = prim;
	self->velocity = gfc_vector3d(0, 0, 0);
	//change this to account for other prim types later
	if (prim.type == GPT_BOX) {
		self->scale = gfc_vector3d(prim.s.b.w, prim.s.b.h, prim.s.b.d);
		self->position = gfc_vector3d(prim.s.b.x, prim.s.b.y, prim.s.b.z);
	}
	else if (prim.type == GPT_SPHERE)
		self->position = gfc_vector3d(prim.s.s.x, prim.s.s.y, prim.s.s.z);
	self->offset = gfc_vector3d(0, 0, 0);
	self->gravity = 0;

	//self->scale = gfc_vector3d(1, 1, 1);
	self->isTrigger = 0;
	self->triggerActive = 0;

	return self;
}

Collider* sphere_collider_new( GFC_Vector3D position, float radius ) {
	GFC_Primitive prim = { 0 };
		prim.s.s = gfc_sphere(position.x, position.y, position.z, radius);
		prim.type = GPT_SPHERE;
	return collider_setup(prim);
}


Collider* plane_collider_new(GFC_Vector3D position, float distance) {
	GFC_Primitive prim = { 0 };
		prim.s.pl = gfc_plane3d(position.x, position.y, position.z, distance);
		prim.type = GPT_PLANE;
	return collider_setup(prim);
}

Collider* box_collider_new(GFC_Vector3D position, GFC_Vector3D dimensions) {
	GFC_Primitive prim = { 0 };
		prim.s.b = gfc_box(position.x, position.y, position.z, dimensions.x, dimensions.y, dimensions.z);
		prim.type = GPT_BOX;
	return collider_setup(prim);
}


Collider* edge_collider_new(GFC_Vector3D position, GFC_Vector3D position2) {
	GFC_Primitive prim = { 0 };
		prim.s.e = gfc_edge3d(position.x, position.y, position.z, position2.x, position2.y, position2.z);
		prim.type = GPT_EDGE;
	return collider_setup(prim);
}


Collider* triangle_collider_new(GFC_Vector3D vertex1, GFC_Vector3D vertex2, GFC_Vector3D vertex3) {
	GFC_Primitive prim = { 0 };
		prim.s.t = gfc_triangle(vertex1, vertex2, vertex3);
		prim.type = GPT_TRIANGLE;
	return collider_setup(prim);
}

void collider_free(Collider* self) {
	//expand when adding sector list and other stuff
}

//TODO make this predictive with velocity not current position?
Uint8 check_collision(Collider* self, Collider* other) {
	if (!self || !other) return 0;
	if (self == other) {
		//slog("no thats me");
		return 0;
	}//do not collide with self

	if (self->primitive.type == GPT_BOX) { // && Layer!= LAYER
		if (other->primitive.type == GPT_BOX) {
			return gfc_box_overlap(self->primitive.s.b, other->primitive.s.b);
		}
		if (other->primitive.type == GPT_SPHERE)
			return 0; //not implemented
		//if (otherother->primitive.type == GPT_PLANE)
	}

	if (self->primitive.type == GPT_SPHERE) { // && Layer!= LAYER
		if (other->primitive.type == GPT_BOX) {
			return 0; //not implemented
		}
		if (other->primitive.type == GPT_SPHERE)
			return gfc_sphere_overlap(self->primitive.s.s, other->primitive.s.s);
		//return gfc_box_overlap(self->primitive.s.b, other->primitive.s.b); not implemented yet
	}
	return 0;
}
void do_collision(Collider* self, Collider* other) {
	 if (!self || !other) return;
	 //should maybe do this in check instead

	 //THIS IS SPHERE 
	 if (self->primitive.type == GPT_SPHERE && other->primitive.type == GPT_SPHERE) {

		 GFC_Vector3D distance;
		 GFC_Vector3D scaledDistance;
		 gfc_vector3d_sub(distance, self->position, other->position);
		 gfc_vector3d_normalize(&distance);
		 //edit scale for collision force/elasticity
		 gfc_vector3d_scale(scaledDistance, distance, 0.035);
		 

		 //use layers to determine if any body is fixed
		 if (self->layer != C_WORLD)
			 gfc_vector3d_add(self->position, self->position, scaledDistance);
		 if (other->layer != C_WORLD)
			 gfc_vector3d_add(other->position, other->position, -scaledDistance);
	 }

	 //THIS IS BOX 
	 //the concept of minimum overlap came from https://youtu.be/oOEnWQZIePs?si=ZMblmzP0ep0f-bJT
	 if (self->primitive.type == GPT_BOX && other->primitive.type == GPT_BOX) {
		 GFC_Vector3D boxDistance;
		 gfc_vector3d_sub(boxDistance, self->position, other->position);
		 
		 float xDist = SDL_fabsf(boxDistance.x);
		 float yDist = SDL_fabsf(boxDistance.y);
		 float zDist = SDL_fabsf(boxDistance.z);
		 //slog("dists: x %f, t %f, z %f", xDist, yDist, zDist);
		 float* max;
		 max = &xDist;
		 if (yDist > *max) max = &yDist;
		 if (zDist > *max) max = &zDist;
		 //slog("max %f", *max);
		 gfc_vector3d_normalize(&boxDistance);
		 gfc_vector3d_scale(boxDistance, boxDistance, 0.05);
		 
		 if (max == &xDist) {
			 slog("do x");
			 if (self->layer != C_WORLD)
				 self->position.x += boxDistance.x;
			 if (other->layer != C_WORLD)
				 other->position.x -= boxDistance.x;
			 return;
		 }
		 else if (max == &yDist) {
			 slog("do y");
			 if (self->layer != C_WORLD)
				 self->position.y += boxDistance.y;
			 if (other->layer != C_WORLD)
				 other->position.y -= boxDistance.y;
			 return;
		 }
		 else if (max == &zDist) {
			 slog("do z");
			 if (self->layer != C_WORLD)
				 self->position.z += boxDistance.z;
			 if (other->layer != C_WORLD)
				 other->position.z -= boxDistance.z;
			 return;
		 }
	 }
	 
}

void collider_update(Collider* self) {
	GFC_Vector3D oldPos = self->position;
	//update stuff
	if (self->velocity.z > -0.035) self->velocity.z-= self->gravity;
	gfc_vector3d_add(self->position, self->position, self->velocity);
	//move primitive
	self->primitive.s.b.x = self->position.x;
	self->primitive.s.b.y = self->position.y;
	self->primitive.s.b.z = self->position.z;
	
	//gfc_primitive_offset(self->primitive, gfc_vector3d_subbed(self->position, oldPos));
}

void set_as_trigger(Collider* self, Uint8 toBeTrigger) {
	if (!self) return;
	self->isTrigger = toBeTrigger;
}