#include "simple_logger.h"
#include "prey.h"

void prey_think(Entity* self);
void prey_update(Entity* self);
void prey_free(Entity* self);

Entity* prey_new()
{
	Entity* self;
	self = entity_new();

	if (!self) {
		slog("failed to create prey entity");
		return NULL;
	}


	//self->name = "playerr";

	//self->position = gfc_vector3d(0, 5, 0);
	self->rotation = gfc_vector3d(0, 0, 0);
	self->scale = gfc_vector3d(0.2, 0.2, 0.2);
	self->model = gf3d_model_load("models/dino.model"); 
	self->collider = box_collider_new(gfc_vector3d(0, 0, -4), gfc_vector3d(1, 1, 1));
	//self->collider = sphere_collider_new(gfc_vector3d(0, 5, 0), 1);
	self->position = self->collider->position;
	self->collider->layer = C_PREY;

	//behavior
	//self->think = prey_think;
	//self->update = prey_update;
	//self->free = prey_free;
	self->draw = NULL;
	self->data = NULL;                            //entity custom data beyond basics

	return self;
}

void prey_think(Entity* self) {

}
void prey_update(Entity* self) {
	
}
void prey_free(Entity* self) {

}
