#include "simple_logger.h"



#include "player.h"


void player_think(Entity* self);
void player_update(Entity* self);
void player_free(Entity* self);

Entity* player_new() 
{
	Entity* self;
	self = entity_new();

	if (!self) {
		slog("failed to create player entity");
		return NULL;
	}

	
	//self->name = "playerr";
	
	self->position = gfc_vector3d(0, 0, 0);
	self->rotation = gfc_vector3d(0, 0, 0);
	//GFC_Vector3D	scale;
	self->model = gf3d_model_load("models/dino.model"); //entity's model if it has one

	//behavior
	self->think = player_think;
	self->update = player_update; 
	self->free = player_free; 
	self->draw = NULL; 
	self->data = NULL;                            //entity custom data beyond basics

	return self;
}

void player_think(Entity* self)
{
	if (!self) return;
}
void player_update(Entity* self)
{
	if (!self) return;
	self->position = gfc_vector3d_added(self->position, gfc_vector3d(0, 0, 0.2));
	if (self->position.z > 60)
		self->position = gfc_vector3d(0, 0, 0);
}
void player_free(Entity* self)
{
	if (!self) return;
}