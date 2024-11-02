#include "simple_logger.h"

#include "gf3d_camera.h"

#include "player.h"


void player_think(Entity* self);
void player_update(Entity* self);
void player_free(Entity* self);
void player_walk_forward(Entity* self, float magnitude);

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
    float moveSpeed = 0.05;
    GFC_Vector3D position, rotation;
    const Uint8* keys;

	if (!self) return;
    /*
	self->position = gfc_vector3d_added(self->position, gfc_vector3d(0, 0, 0.2));
	if (self->position.z > 60)
		self->position = gfc_vector3d(0, 0, 0);
   */
        

        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

      
            if (keys[SDL_SCANCODE_B])
            {
               // self->rotation = gf3d_camera_get_angles();
               // gfc_vector3d_rotate_about_z(&self->rotation, GFC_PI); this func didnt work
                self->rotation.z+= GFC_HALF_PI/100;
                slog("rotation: %f,%f,%f", self->rotation.x, self->rotation.y, self->rotation.z);
            }
         
            if (keys[SDL_SCANCODE_N])
            {
                player_walk_forward(self, moveSpeed);
            }
            else
            {
                self->velocity = gfc_vector3d(0,0,0);
            }
            
            if (keys[SDL_SCANCODE_M])
            {
                player_walk_forward(self, -moveSpeed);
            }
            
            if (keys[SDL_SCANCODE_V])
            {
                self->rotation = gf3d_camera_get_angles();
                self->rotation.z = -self->rotation.z;
                self->rotation.z += GFC_PI;
                
            }
            /*
            if (keys[SDL_SCANCODE_A])
            {
                gf3d_camera_walk_right(-moveSpeed);
            }
            if (keys[SDL_SCANCODE_SPACE])gf3d_camera_move_up(moveSpeed);
          */

            return;
        
       
    
}
void player_free(Entity* self)
{
	if (!self) return;
}

void player_move(Entity* self, GFC_Vector3D translation)
{
    gfc_vector3d_sub(self->position, self->position, translation);
}
void player_walk_forward(Entity* self, float magnitude)
{
    GFC_Vector2D w;
    GFC_Vector3D forward = { 0 };
    w = gfc_vector2d_from_angle(-self->rotation.z);
    forward.x = -w.x;
    forward.y = -w.y;
    gfc_vector3d_set_magnitude(&forward, magnitude);
    self->velocity = forward;

}

void cam_follow_player(Entity* self, float offset) 
{
    GFC_Vector2D w;
    GFC_Vector3D forward = { 0 };
    w = gfc_vector2d_from_angle(-self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    //gfc_vector3d_normalize
   // gf3d_camera_look_at(gfc_vector3d(0, -10, 0), NULL);
}