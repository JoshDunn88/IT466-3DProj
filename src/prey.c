#include "simple_logger.h"
#include "prey.h"

void prey_think(Entity* self);
void prey_update(Entity* self);
void prey_free(Entity* self);
void prey_eaten(Collider* self, Collider* other);
void prey_chillin(Collider* self, Collider* other);

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
	self->collider = box_collider_new(gfc_vector3d(0, 5, 0), gfc_vector3d(1, 1, 1));
	//self->collider = sphere_collider_new(gfc_vector3d(0, 5, 0), 1);
	self->position = self->collider->position;
	self->collider->layer = C_PREY;
	self->collider->isTrigger = 1;
	self->collider->triggerActive = 0;
	self->collider->onTriggerEnter = prey_eaten;
	self->collider->onTriggerExit = prey_chillin;
	self->collider->whileTrigger = prey_chillin;

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

void prey_eaten(Collider* self, Collider* other) {
	
	//slog("bout to bea eaten");
	if (!self || !other) {
		slog("one of us isn't real");
		return;
	}

	if (other->layer != C_PLAYER) {
		slog("you cant eat me");
		return;
	}

	Entity* selfEnt;
	Entity* playerEnt;
	selfEnt = entity_get_by_collider(self);
	playerEnt = entity_get_by_collider(other);

	if (!selfEnt||!playerEnt) {
		slog("failed to get entity");
		return;
	}

	
	
	//this is probably bad
	if (selfEnt->alive) {
		slog("i've been eaten");
		selfEnt->alive = 0;
		return;
	}
	slog("why am i here");
}

//mandatory for trigger functionality
void prey_chillin(Collider* self, Collider* other) {
	//chillin
}