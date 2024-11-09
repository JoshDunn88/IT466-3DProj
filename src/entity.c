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


void check_collisions(Collider* self) {
	int i;
	Uint8 collided = 0;
	for (i = 0; i < _entity_manager.entityMax; i++) {
		if (!_entity_manager.entityList[i]._inuse) continue;
		if (!_entity_manager.entityList[i].alive) continue;
		//skip same layer collisions
		if (self->layer == _entity_manager.entityList[i].collider->layer) return;

		collided = check_collision(self, _entity_manager.entityList[i].collider);
		if (collided) slog("collided");

		if (collided && !self->isTrigger) {
			//die
			if (self->layer == C_PREDATOR && _entity_manager.entityList[i].collider->layer == C_PLAYER){
				_entity_manager.entityList[i].alive = 0;
				}
			do_collision(self, _entity_manager.entityList[i].collider); //do you need to do this in reverse also??
			continue;
		}
		//redo this horrendous shit later
		if (!self->isTrigger) continue;

			if (collided) {
				if (!self->triggerActive) {
					slog("about to trigger");
					self->onTriggerEnter(self, _entity_manager.entityList[i].collider);
					self->triggerActive = 1;
					continue;
				}
				self->whileTrigger(self, _entity_manager.entityList[i].collider);
			}
			else {
				//slog("no more trigger");
				if (self->triggerActive) {
					self->onTriggerExit(self, _entity_manager.entityList[i].collider);
					self->triggerActive = 0;
					continue;
				}
			}
			
		

	}
}

Entity* entity_get_by_collider(Collider* self) {
	if (!self) return NULL;
	int i;
	for (i = 0; i < _entity_manager.entityMax; i++) {
		if (!_entity_manager.entityList[i]._inuse) continue;
		if (_entity_manager.entityList[i].collider == self) return &_entity_manager.entityList[i];

	}
	return NULL;
}

void entity_clear_all(Entity* ignore) 
{
	int i;
	for (i = 0; i < _entity_manager.entityMax; i++) {
		if (&_entity_manager.entityList[i] == ignore) continue;
		//this was wrong?
		//if (!_entity_manager.entityList[i]._inuse) continue;
		//do specific free
		entity_free(&_entity_manager.entityList[i]);
		
	}
}

void entity_think_all() 
{
	int i;
	for (int i = 0; i < _entity_manager.entityMax; i++) {
		if (!_entity_manager.entityList[i]._inuse) continue;
		if (!_entity_manager.entityList[i].alive) continue;
		entity_think(&_entity_manager.entityList[i]);
	}
}


void entity_update_all()
{
	int i;
	for (int i = 0; i < _entity_manager.entityMax; i++) {
		if (!_entity_manager.entityList[i]._inuse) continue;
		if (!_entity_manager.entityList[i].alive) continue;
		entity_update(&_entity_manager.entityList[i]);
	}
}

void entity_draw_all()
{
	int i;
	for (i = 0; i < _entity_manager.entityMax; i++) {
		if (!_entity_manager.entityList[i]._inuse) continue;
		if (!_entity_manager.entityList[i].alive) continue;
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
		_entity_manager.entityList[i].alive = 1;
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
	slog("model freed");
	if (self->collider) free(self->collider);
	slog("collider freed");
	//free anthing special that may have been allocated FOR this
	if (self->free) self->free(self->data);
	slog("data freed");
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
	

	if (self->collider) {
		check_collisions(self->collider);
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
	GFC_Color mineColor;
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
	mineColor = gfc_color(0.5, 0.5, 0.4, 0.5);
	if (self->collider->layer == C_WORLD) mineColor = gfc_color(0.8, 0.5, 0.2, 1);
	if (self->collider->layer == C_PLAYER) mineColor = gfc_color(0.1, 0.2, 0.8, 0.4);
	if (self->collider->layer == C_PREY) mineColor = gfc_color(0.1, 0.8, 0.2, 0.4);
	if (self->collider->layer == C_PREDATOR) mineColor = gfc_color(0.9, 0.1, 0.1, 0.4);
	if (self->collider->primitive.type == GPT_BOX) {
		//if (self->collider->layer == C_PLAYER) slog("prim before: %f, %f, %f, %f, %f, %f", self->collider->primitive.s.b.x, self->collider->primitive.s.b.y, self->collider->primitive.s.b.z, self->collider->primitive.s.b.w, self->collider->primitive.s.b.h, self->collider->primitive.s.b.d);
		gf3d_draw_cube_solid(self->collider->primitive.s.b, gfc_vector3d(self->collider->primitive.s.b.x, self->collider->primitive.s.b.y, self->collider->primitive.s.b.z), gfc_vector3d(0, 0, 0), self->collider->scale, mineColor);
		//if (self->collider->layer == C_PLAYER) slog("prim after: %f, %f, %f, %f, %f, %f", self->collider->primitive.s.b.x, self->collider->primitive.s.b.y, self->collider->primitive.s.b.z, self->collider->primitive.s.b.w, self->collider->primitive.s.b.h, self->collider->primitive.s.b.d);
	}
	if (self->collider->primitive.type == GPT_SPHERE) gf3d_draw_sphere_solid(self->collider->primitive.s.s, self->collider->position, gfc_vector3d(0, 0, 0), gfc_vector3d(2, 2, 2), mineColor, gfc_color(0, 0, 0, 0));

}




