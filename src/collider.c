
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
	self->offset = gfc_vector3d(0, 0, 0);
	//self->scale = gfc_vector3d(1, 1, 1);
	self->isTrigger = 0;

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

//TODO make this predictive with velocity not current position
Uint8 check_collision(Collider* self, Collider* other) {
	if (!self || !other) return 0;
	if (self->primitive.type == GPT_BOX) { // && Layer!= LAYER
		if (other->primitive.type == GPT_BOX)
			return gfc_box_overlap(self->primitive.s.b, other->primitive.s.b);
		if (other->primitive.type == GPT_SPHERE)
			return 0; 
			//return gfc_box_overlap(self->primitive.s.b, other->primitive.s.b); not implemented yet
	}
	return 0;
}
void do_collision(Collider* self, Collider* other) {
	 if (!self || !other) return;
	 if (self->isTrigger) {
		 self->onTriggerEnter(self, other);
		 return;
	 }
	 /* shouldn't need this collisions dhould be do_collision twice per collision switching self and other
	 if (other->isTrigger) {
		 self->onTriggerEnter(other, self);
	 }
	 */

	 //do physics collision
}

void collider_update(Collider* self) {
	//update stuff
	gfc_vector3d_add(self->position, self->position, self->velocity);
}

void set_as_trigger(Collider* self, Uint8 toBeTrigger) {
	if (!self) return;
	self->isTrigger = toBeTrigger;
}