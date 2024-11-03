#include "simple_logger.h"

#include "gf3d_camera.h"

#include "player.h"
#include "gfc_primitives.h" //shouldn't need this


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
	self->scale = gfc_vector3d(0.5, 0.5, 0.5);
	self->model = gf3d_model_load("models/dino.model"); //entity's model if it has one
    self->collider = box_collider_new(self->position, gfc_vector3d(1,1,1));
    self->collider->layer = C_PLAYER;

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

      
            if (keys[SDL_SCANCODE_A])
            {
               // self->rotation = gf3d_camera_get_angles();
               // gfc_vector3d_rotate_about_z(&self->rotation, GFC_PI); this func didnt work
                self->rotation.z+= 0.01;
                gfc_angle_clamp_radians(&self->rotation.z);
                slog("rotation: %f,%f,%f", self->rotation.x, self->rotation.y, self->rotation.z);
                //slog("position: %f,%f,%f", self->position.x, self->position.y, self->position.z);
            }
         
            if (keys[SDL_SCANCODE_W])
            {
                player_walk_forward(self, moveSpeed);
            }
            else
            {
                if (self->collider)
                    self->collider->velocity = gfc_vector3d(0,0,0);
            }
            
            if (keys[SDL_SCANCODE_S])
            {
                player_walk_forward(self, -moveSpeed);
            }
            
            if (keys[SDL_SCANCODE_D])
            {
                // self->rotation = gf3d_camera_get_angles();
               // gfc_vector3d_rotate_about_z(&self->rotation, GFC_PI); this func didnt work
                self->rotation.z -= 0.01;
                gfc_angle_clamp_radians(&self->rotation.z);
                slog("rotation: %f,%f,%f", self->rotation.x, self->rotation.y, self->rotation.z);
                //slog("position: %f,%f,%f", self->position.x, self->position.y, self->position.z);
            }
            /*
            * TODO: Rotate with mouse, strafe with a/d
            if (keys[SDL_SCANCODE_A])
            {
                gf3d_camera_walk_right(-moveSpeed);
            }
            TODO: Jump
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
    if (!self->collider) {
        slog("player has no collider to move");
        return;
    }
    GFC_Vector2D w;
    GFC_Vector3D forward = { 0 };

    w = gfc_vector2d_from_angle(self->rotation.z);
    forward.x = -w.x;
    forward.y = -w.y;
    gfc_vector3d_set_magnitude(&forward, magnitude);
    
    self->collider->velocity = forward;

}

void cam_follow_player(Entity* self, float offset) 
{
    GFC_Vector2D w;
    GFC_Vector3D forward = { 0 };
    GFC_Vector3D offsetPos = { 0 };

    w = gfc_vector2d_from_angle(self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    gfc_vector3d_normalize(&forward);
    //slog("befforward: %f,%f,%f", forward.x, forward.y, forward.z);
    gfc_vector3d_scale(forward,forward, offset);
    //slog("aftforward: %f,%f,%f", forward.x, forward.y, forward.z);
   //slog("forward: %f,%f,%f", forward.x, forward.y, forward.z);

    gfc_vector3d_sub(offsetPos, self->position, forward);
    
    gfc_vector3d_copy(forward, self->position);
    forward.z += 1.5;
    offsetPos.z += 5;
   // slog("position: %f,%f,%f", offsetPos.x, offsetPos.y, offsetPos.z);
    gf3d_camera_look_at(forward, &offsetPos);
    
}