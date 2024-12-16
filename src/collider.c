
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
	else if (prim.type == GPT_SPHERE) {
		self->position = gfc_vector3d(prim.s.s.x, prim.s.s.y, prim.s.s.z);
		//change later is bad
		self->scale = gfc_vector3d(prim.s.s.r, prim.s.s.r, prim.s.s.r);
	}
		
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

Collider* box_collider_new(GFC_Vector3D position, GFC_Vector3D dimensions) {
	GFC_Primitive prim = { 0 };
		prim.s.b = gfc_box(position.x - dimensions.x/2, position.y - dimensions.y / 2, position.z - dimensions.z / 2, dimensions.x, dimensions.y, dimensions.z);
		prim.type = GPT_BOX;
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

	 //special cases
	
	 //REDO THIS THIS IS OLD, use the actual distance like in box
	 //THIS IS SPHERE 
	 if (self->primitive.type == GPT_SPHERE && other->primitive.type == GPT_SPHERE) {

		 GFC_Vector3D distance;
		 GFC_Vector3D scaledDistance;
		 gfc_vector3d_sub(distance, self->position, other->position);
		 gfc_vector3d_normalize(&distance);
		 //edit scale for collision force/elasticity
		 //gfc_vector3d_scale(scaledDistance, distance, 0.035);
		 

		 //use layers to determine if any body is fixed
		 if (self->layer != C_WORLD)
			 gfc_vector3d_add(self->position, self->position, self->primitive.s.s.r + other->primitive.s.s.r - distance);
		 if (other->layer != C_WORLD)
			 gfc_vector3d_add(other->position, other->position, self->primitive.s.s.r + other->primitive.s.s.r - distance);

		 return;
	 }

	 //THIS IS BOX 
	 //the concept of minimum overlap came from https://youtu.be/oOEnWQZIePs?si=ZMblmzP0ep0f-bJT
	 if (self->primitive.type == GPT_BOX && other->primitive.type == GPT_BOX) {
		 GFC_Vector3D boxDistance;
		 gfc_vector3d_sub(boxDistance, self->position, other->position);
		 
		 float xDist = SDL_fabsf(boxDistance.x);
		 float yDist = SDL_fabsf(boxDistance.y);
		 float zDist = SDL_fabsf(boxDistance.z);
		 float xDistRel = xDist / (self->scale.x / 2 + other->scale.x / 2);
		 float yDistRel = yDist / (self->scale.y / 2 + other->scale.y / 2);
		 float zDistRel = zDist / (self->scale.z / 2 + other->scale.z / 2);
		 //slog("dists: x %f, y %f, z %f", xDist, yDist, zDist);
		 float* max;
		 max = &xDistRel;
		 if (yDistRel > *max) max = &yDistRel;
		 if (zDistRel > *max) max = &zDistRel;
		 //slog("max %f", *max);
		 
		 if (max == &xDistRel) {
			 //slog("do x");
			 if (self->layer != C_WORLD) {
				
				 if (boxDistance.x > 0)
					self->position.x += (self->scale.x / 2 + other->scale.x / 2) - xDist + 0.001f;
				 else
					 self->position.x -= (self->scale.x / 2 + other->scale.x / 2) - xDist + 0.001f;
				 self->velocity.x /= 2;
			 }
				 
			 else {
				 if (boxDistance.x > 0)
					 other->position.x -= (self->scale.x / 2 + other->scale.x / 2) - xDist + 0.001f;
				 else
					 other->position.x += (self->scale.x / 2 + other->scale.x / 2) - xDist + 0.001f;
				 other->velocity.x = self->velocity.x;
			 }
				 
			 return;
		 }
		 else if (max == &yDistRel) {
			 //slog("do y");
			 if (self->layer != C_WORLD) {
				 
					 if (boxDistance.y > 0)
						 self->position.y += (self->scale.y / 2 + other->scale.y / 2) - yDist + 0.001f;
					 else
						 self->position.y -= (self->scale.y / 2 + other->scale.y / 2) - yDist + 0.001f;
				 
				 self->velocity.y /= 2;
			 }	
			 else {
				 if (boxDistance.y > 0)
					 other->position.y -= (self->scale.y / 2 + other->scale.y / 2) - yDist + 0.001f;
				 else
					 other->position.y += (self->scale.y / 2 + other->scale.y / 2) - yDist + 0.001f;
				 other->velocity.y = self->velocity.y;
			 }
			 return;
		 }
		 else if (max == &zDistRel) {
			 //slog("do z");
			 if (self->layer != C_WORLD) {
				 
				 if (boxDistance.z > 0)
					self->position.z += (self->scale.z / 2 + other->scale.z / 2) - zDist + 0.001f;
				 else
					 self->position.z -= (self->scale.z / 2 + other->scale.z / 2) - zDist + 0.001f;
				 self->velocity.z /= 2;
			 }
			 //if me ground move you
			 else {
				 if (boxDistance.z > 0)
					 other->position.z -= (self->scale.z / 2 + other->scale.z / 2) - zDist + 0.001f;
				 else
					 other->position.z += (self->scale.z / 2 + other->scale.z / 2) - zDist + 0.001f;
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
	gfc_vector3d_add(self->position, self->position, self->velocity);

	if (self->velocity.z > -0.1) self->velocity.z-= self->gravity;
	//friction
	self->velocity.x *= 0.6f;
	self->velocity.y *= 0.6f;
	//move primitive this is for boxes
	if (self->primitive.type == GPT_BOX) {
		self->primitive.s.b.x = self->position.x - self->primitive.s.b.w / 2;
		self->primitive.s.b.y = self->position.y - self->primitive.s.b.h / 2;
		self->primitive.s.b.z = self->position.z - self->primitive.s.b.d / 2;
	}
	else if (self->primitive.type == GPT_SPHERE) {
		self->primitive.s.s.x = self->position.x;
		self->primitive.s.s.y = self->position.y;
		self->primitive.s.s.z = self->position.z;
	}
	
	//gfc_primitive_offset(self->primitive, gfc_vector3d_subbed(self->position, oldPos));
}

void set_as_trigger(Collider* self, Uint8 toBeTrigger) {
	if (!self) return;
	self->isTrigger = toBeTrigger;
}