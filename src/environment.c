#include "simple_logger.h"
#include "environment.h"




Entity* environment_new() {

	Entity* self;
	self = entity_new();

	if (!self) {
		slog("failed to create env entity");
		return NULL;
	}


	//self->name = "playerr";

	//self->position = gfc_vector3d(0, 5, 0);
	self->rotation = gfc_vector3d(0, 0, 0);
	self->scale = gfc_vector3d(1, 1, 1);
	self->model = NULL; //entity's model if it has one
	self->collider = box_collider_new(gfc_vector3d(1, -5, 0.1), gfc_vector3d(1, 1, 1));
	self->position = self->collider->position;
	self->collider->layer = C_WORLD;
	self->collider->isTrigger = 0;

	//behavior
	self->think = NULL;
	self->update = NULL;
	self->free = NULL;
	self->draw = NULL;
	self->data = NULL;                            //entity custom data beyond basics

	return self;

}