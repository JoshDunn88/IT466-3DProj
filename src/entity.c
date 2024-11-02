#include "simple_logger.h"
#include "gfc_matrix.h"
#include "entity.h"

typedef struct
{
	Entity* entityList;
	Uint32 entityMax;
}EntityManager;

static EntityManager _entity_manager = { 0 }; //initialize local global entity manager

void entity_system_close() {

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
		//need to free the entity pointer itself somewhere too
		//if (_entity_manager.entityList[i].free) _entity_manager.entityList[i].free(&_entity_manager.entityList[i]); //changed to pass self? is .data in vid
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
		return &_entity_manager.entityList[i];
	}
	slog("no more open entity slots");
	return NULL; //no more entity slots

}

void entity_free(Entity* self)
{
	if (!self) return;
	gf3d_model_free(self->model);

	//free anthing special that may have been allocated FOR this
	if (self->free) self->free(self->data);
	//free self itself??
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


}




