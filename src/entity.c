#include "simple_logger.h"
#include "gfc_matrix.h"
#include "entity.h"

typedef struct
{
	Entity* entityList;
	Uint32 entityMax;
}EntityManager;

static EntityManager _entity_manager = { 0 }; //initialize local global entity manager

void entity_system_close(void) {

	entity_clear_all(NULL);
	if (_entity_manager.entityList) free(_entity_manager.entityList);
	memset(&_entity_manager, 0, sizeof(EntityManager));
}

void entity_system_init(Uint32 maxEnts) 
{
	if (!maxEnts) 
	{
		slog("cannot allocate 0 entities");
	}
	if (_entity_manager.entityList) {
		slog("entity manager already exists");
		return;
	}
	_entity_manager.entityList = gfc_allocate_array(sizeof(Entity), maxEnts);
	if (!_entity_manager.entityList) {
		slog("failed to allocate %i entities for the entity manager", maxEnts);
	}
	_entity_manager.entityMax = maxEnts;
	atexit(entity_system_close);
}



void entity_clear_all(Entity* ignore) 
{
	int i;
	for (int i = 0; i < _entity_manager.entityMax; i++) {
		if (&_entity_manager.entityList[i] == ignore) continue;
		if (!_entity_manager.entityList[i]._inuse) continue;
		//do specific free
		entity_free(&_entity_manager.entityList[i]);
		
	}
}

void entity_think_all() 
{
	int i;
	for (int i = 0; i < _entity_manager.entityMax; i++) {
		if (!_entity_manager.entityList[i]._inuse) continue;
		entity_think(&_entity_manager.entityList[i]);
	}
}

void check_collisions(Collider* self) {
	int i;
	Uint8 collided = 0;
	for (int i = 0; i < _entity_manager.entityMax; i++) {
		if (!_entity_manager.entityList[i]._inuse) continue;
		collided = check_collision(self, _entity_manager.entityList[i].collider);
		if (collided) slog("collided");

		if (collided && !self->isTrigger) {
			do_collision(self, _entity_manager.entityList[i].collider); //do you need to do this in reverse also??
			return;
		}
		//redo this horrendous shit later
		if (self->isTrigger) {
			if (collided){
				if (!self->triggerActive) {
					self->onTriggerEnter(self, _entity_manager.entityList[i].collider);
					self->triggerActive = 1;
					return;
				}
				self->whileTrigger(self, _entity_manager.entityList[i].collider);
			}
			else if(self->triggerActive){
				self->onTriggerExit(self, _entity_manager.entityList[i].collider);
				self->triggerActive = 0;
				return;
			}
			return;
		}
		
	}
}

void entity_update_all()
{
	int i;
	for (int i = 0; i < _entity_manager.entityMax; i++) {
		if (!_entity_manager.entityList[i]._inuse) continue;
		entity_update(&_entity_manager.entityList[i]);
	}
}

void entity_draw_all()
{
	int i;
	for (i = 0; i < _entity_manager.entityMax; i++) {
		if (!_entity_manager.entityList[i]._inuse) continue;
		//do draw
		entity_draw(&_entity_manager.entityList[i]);
	}

}

Entity* entity_new()
{
	int i;
	for (i = 0; i < _entity_manager.entityMax; i++)
	{
		if (_entity_manager.entityList[i]._inuse) continue;//skip ones in use
		memset(&_entity_manager.entityList[i], 0, sizeof(Entity));
		_entity_manager.entityList[i]._inuse = 1;
		_entity_manager.entityList[i].scale = gfc_vector3d(1, 1, 1);
		_entity_manager.entityList[i].collider = NULL;
		//_entity_manager.entityList[i].velocity = gfc_vector3d(0, 0, 0);
		return &_entity_manager.entityList[i];
	}
	slog("no more open entity slots");
	return NULL; //no more entity slots

}

void entity_free(Entity* self)
{
	if (!self) return;
	gf3d_model_free(self->model);
	if (self->collider) free(self->collider);
	//free anthing special that may have been allocated FOR this
	if (self->free) self->free(self->data);
}

void entity_think (Entity* self)
{
	if (!self) return;
	//basic think stuff here before self think
	//methink
	if (self->think) self->think(self);
}

void entity_update(Entity* self)
{
	if (!self) return;
	//basic think stuff here before self update
	//physics movement
	check_collisions(self->collider);

	if (self->collider) {
		collider_update(self->collider);
		//I have absoluetely no idea why this is necessary to multiply by, is there a bit shift happening somewhere I don't know about??
		gfc_vector3d_scale(self->position, self->collider->position, 2);
		
	}
	//meupdate
	if (self->update) self->update(self);
}

void entity_draw(Entity *self)
{
	GFC_Matrix4 matrix;
	if (!self) return;

	//this is weird, old code commented out
	//if (self->draw(self) == -1) return;
	if (self->draw) {
		self->draw(self);
		return;
	}
	gfc_matrix4_from_vectors(
		matrix,
		self->position,
		self->rotation,
		self->scale
	);

	gf3d_model_draw(
		self->model,
		matrix,
		GFC_COLOR_WHITE,
		0
	);
	//draw bounding box
	if (!self->collider)return;
	if (self->collider->primitive.type == GPT_BOX) gf3d_draw_cube_solid(self->collider->primitive.s.b, gfc_vector3d(self->collider->primitive.s.b.x, self->collider->primitive.s.b.y, self->collider->primitive.s.b.z), gfc_vector3d(0, 0, 0), gfc_vector3d(1, 1, 1), gfc_color(0.8, 0.2, 0.1, 0.4));
	//if (self->collider->primitive.type==GPT_BOX) gf3d_draw_cube_solid(self->collider->primitive.s.b, self->collider->position, gfc_vector3d(0, 0, 0), gfc_vector3d(1, 1, 1), gfc_color(0.8, 0.2, 0.1, 0.4));
	//if (self->collider->primitive.type == GPT_SPHERE) gf3d_draw_sphere_solid(self->collider->primitive.s.s, self->collider->position, gfc_vector3d(0, 0, 0), gfc_vector3d(2, 2, 2), gfc_color(0.8, 0.5, 0.1, 0.4), gfc_color(0, 0, 0, 0));

}




