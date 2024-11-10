
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
		self->position = gfc_vector3d(prim.s.b.x + prim.s.b.w/2, prim.s.b.y + prim.s.b.h/2, prim.s.b.z + prim.s.b.d/2);
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
		prim.s.b = gfc_box(position.x - dimensions.x/2, position.y - dimensions.y / 2, position.z - dimensions.z / 2, dimensions.x, dimensions.y, dimensions.z);
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
Uint8 check_collision(Collider* self, Collider* other, GFC_Vector3D self_vel, GFC_Vector3D other_vel) {
	if (!self || !other) return 0;
	if (self == other) {
		//slog("no thats me");
		return 0;
	}//do not collide with self

	if (self->primitive.type == GPT_BOX) { // && Layer!= LAYER
		if (other->primitive.type == GPT_BOX) {
			return predictive_box_overlap(self->primitive.s.b, other->primitive.s.b, self_vel, other_vel);
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

	 //special cases
	
	 //do we need to add iterative velocity instead?
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
		 slog("dists: x %f, y %f, z %f", xDist, yDist, zDist);
		 float* max;
		 max = &xDist;
		 if (yDist > *max) max = &yDist;
		 if (zDist > *max) max = &zDist;
		 slog("max %f", *max);
		 //gfc_vector3d_normalize(&boxDistance);
		 //gfc_vector3d_scale(boxDistance, boxDistance, 0.05);
		 // probably need to do position too
		 if (max == &xDist) {
			 slog("do x");
			 if (self->layer != C_WORLD) {
				 //self->velocity.x = -self->velocity.x;
				 self->velocity.x /= 2;
				 if (other->layer != C_WORLD) {
					 if (boxDistance.x > 0) {
						 self->position.x -= ((self->scale.x / 2 + other->scale.x / 2) - xDist) / 2;
						 other->position.x += ((self->scale.x / 2 + other->scale.x / 2) - xDist) / 2;
					 }
					 else {
						 self->position.x += ((self->scale.x / 2 + other->scale.x / 2) - xDist) / 2;
						 other->position.x -= ((self->scale.x / 2 + other->scale.x / 2) - xDist) / 2;
					 }
					 other->velocity.x = self->velocity.x; //maybe not do
				 }
				 else {
					 if (boxDistance.x > 0) {
						 self->position.x -= ((self->scale.x / 2 + other->scale.x / 2) - xDist);

					 }
					 else {
						 self->position.x += ((self->scale.x / 2 + other->scale.x / 2) - xDist);

					 }
				 }


			 }
			 /* do I really need to do both? its already doubled in the loop check
			  if (other->layer != C_WORLD) {
				  other->velocity.y = self->velocity.y;
			  }
			  */
			 return;
		 }
		 else if (max == &yDist) {
			 slog("do y");
			 if (self->layer != C_WORLD) {
				 //self->velocity.x = -self->velocity.x;
				 self->velocity.y /= 2;
				 if (other->layer != C_WORLD) {
					 if (boxDistance.y > 0) {
						 self->position.y -= ((self->scale.y / 2 + other->scale.y / 2) - yDist) + 0.001f;
						 other->position.y += ((self->scale.y / 2 + other->scale.y / 2) - yDist) + 0.001f;
					 }
					 else {
						 self->position.y += ((self->scale.y / 2 + other->scale.y / 2) - yDist) + 0.001f;
						 other->position.y -= ((self->scale.y / 2 + other->scale.y / 2) - yDist) + 0.001f;
					 }
					 other->velocity.y = self->velocity.y; //maybe not do
				 }
				 else {
					 if (boxDistance.y > 0) {
						 self->position.y -= ((self->scale.y / 2 + other->scale.y / 2) - yDist);
						 
					 }
					 else {
						 self->position.y += ((self->scale.y / 2 + other->scale.y / 2) - yDist);
					
					 }
				 }
					
				 
			 }
			/* do I really need to do both? its already doubled in the loop check
			 if (other->layer != C_WORLD) {
				 other->velocity.y = self->velocity.y;
			 }
			 */
			 return;
		 }
		 else if (max == &zDist) {
			 slog("do z");
			 if (self->layer != C_WORLD) {
				 //self->velocity.x = -self->velocity.x;
				 if (boxDistance.z > 0)
					self->position.z -= (self->scale.z / 2 + other->scale.z / 2) - zDist;
				 else
					 self->position.z += (self->scale.z / 2 + other->scale.z / 2) - zDist;
				 self->velocity.z /= 2;
			 }

			 if (other->layer != C_WORLD) {
				 other->velocity.z = self->velocity.z;
			 }
			 return;
		 }
	 }
	 
}

Uint8 predictive_box_overlap(GFC_Box a, GFC_Box b, GFC_Vector3D a_d, GFC_Vector3D b_d)
{
	if ((a.x + a_d.x > b.x + b_d.x + b.w) || (b.x + b_d.x > a.x + a_d.x + a.w) ||
		(a.y + a_d.y > b.y + b_d.y + b.h) || (b.y + b_d.y > a.y + a_d.y + a.h) ||
		(a.z + a_d.z > b.z + b_d.z + b.d) || (b.z + b_d.z > a.z + a_d.z + a.d))
	{
		return 0;
	}
	return 1;
}

void collider_update(Collider* self) {
	GFC_Vector3D oldPos = self->position;
	//update stuff
	if (self->velocity.z > -0.035) self->velocity.z-= self->gravity;
	gfc_vector3d_add(self->position, self->position, self->velocity);
	//move primitive
	self->primitive.s.b.x = self->position.x - self->primitive.s.b.w /2;
	self->primitive.s.b.y = self->position.y - self->primitive.s.b.h /2;
	self->primitive.s.b.z = self->position.z - self->primitive.s.b.d /2;
	
	//gfc_primitive_offset(self->primitive, gfc_vector3d_subbed(self->position, oldPos));
}

void set_as_trigger(Collider* self, Uint8 toBeTrigger) {
	if (!self) return;
	self->isTrigger = toBeTrigger;
}