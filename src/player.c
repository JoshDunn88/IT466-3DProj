#include "simple_logger.h"

#include "gf3d_camera.h"
#include "gf2d_mouse.h"

#include "player.h"
//#include "gfc_primitives.h" //shouldn't need this


void player_think(Entity* self);
void player_update(Entity* self);
void player_free(void* data);
void player_walk_forward(Entity* self, float magnitude);

//boooo global variable bad
Uint8 g;
Uint8 SPACE;
GFC_Vector2D temp;
float maxSpeed = 0.06;

Player_Data* player_data_new() {
    Player_Data* data = malloc(sizeof(Player_Data));

    if (!data) {
        slog("failed to create player data");
        return NULL;
    }
    data->health = 100;
    data->prey_eaten = 0;
    data->cam_target = gfc_vector3d(0, 0, 0);
    return data;
}

Entity* player_new() 
{
	Entity* self;
	self = entity_new();

	if (!self) {
		slog("failed to create player entity");
		return NULL;
	}

	
	//self->name = "playerr";
    float capsule_radius = 1;
	self->position = gfc_vector3d(0, 0, 0);
	self->rotation = gfc_vector3d(0, 0, 0);
	self->scale = gfc_vector3d(2, 2, 2);
	self->model = gf3d_model_load("models/player.model"); 
    //self->collider = box_collider_new(self->position, gfc_vector3d(2.6, 2.6, 2.6));
    self->collider = sphere_collider_new(self->position, capsule_radius);
    self->collider->layer = C_PLAYER;
    self->collider->gravity = 0.00;

    self->capsule = capsule_new(self->position, gfc_vector3d(0, 1, 0), 2, capsule_radius);
	//behavior
	self->think = player_think;
	self->update = player_update; 
	self->free = player_free; 
	self->draw = player_draw;
	self->data = player_data_new();                            //entity custom data beyond basics

	return self;
}

void player_think(Entity* self)
{
	if (!self) return;
    Player_Data* dat = (struct Player_Data*)(self->data);
    if (!dat) return;
    //slog("health: %i, consumed: %i", dat->health, dat->prey_eaten);
    if (!self->collider) return;
    check_world_bounds(self->collider);
}
void player_update(Entity* self)
{
    temp = gfc_vector2d(self->collider->velocity.x, self->collider->velocity.y);
    float moveSpeed = 0.02;
   
    GFC_Vector3D position, rotation;
    const Uint8* keys;
	if (!self) return;
    if (!self->collider) return;
             
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

            if (keys[SDL_SCANCODE_A])
            {
                player_walk_right(self, moveSpeed);
            }
         
            if (keys[SDL_SCANCODE_W])
            {
                player_walk_forward(self, moveSpeed);
            }
            
            if (keys[SDL_SCANCODE_S])
            {
                player_walk_forward(self, -moveSpeed);
            }
            
            if (keys[SDL_SCANCODE_D])
            {
                player_walk_right(self, -moveSpeed);
            }

            //do accumulated move + clamp
            if (gfc_vector2d_magnitude_squared(temp) <= maxSpeed * maxSpeed || gfc_vector2d_magnitude_squared(temp) <= gfc_vector2d_magnitude_squared(gfc_vector2d(self->collider->velocity.x, self->collider->velocity.y)))
            {
                self->collider->velocity.x = temp.x;
                self->collider->velocity.y = temp.y;
            }
            
            
            //keydown only
            if (keys[SDL_SCANCODE_G])
            {
                
                if (self && self->collider && !g) {
                    if (!self->collider->gravity) self->collider->gravity = 0.002;
                    else self->collider->gravity = 0;
                    slog("gravity time");
                    g = 1;
                }
            }
            else if (g) { g = 0;}

            if (keys[SDL_SCANCODE_Q])
            {
                self->rotation.z += 0.02;
                //self->capsule->rotation.z += 0.02;
                gfc_angle_clamp_radians(&self->rotation.z);
                //slog("rotation: %f,%f,%f", self->rotation.x, self->rotation.y, self->rotation.z);
            }

            if (keys[SDL_SCANCODE_E])
            {
                self->rotation.z -= 0.02;
                gfc_angle_clamp_radians(&self->rotation.z);
               // slog("rotation: %f,%f,%f", self->rotation.x, self->rotation.y, self->rotation.z);
            }

            if (keys[SDL_SCANCODE_Z])
            {
                self->rotation.y += 0.02;
                gfc_angle_clamp_radians(&self->rotation.y);
                //slog("rotation: %f,%f,%f", self->rotation.x, self->rotation.y, self->rotation.z);
            }

            if (keys[SDL_SCANCODE_X])
            {
                self->rotation.y -= 0.02;
                gfc_angle_clamp_radians(&self->rotation.y);
                //slog("rotation: %f,%f,%f", self->rotation.x, self->rotation.y, self->rotation.z);
            }
            if (keys[SDL_SCANCODE_T])
            {
                self->rotation.x += 0.02;
                gfc_angle_clamp_radians(&self->rotation.y);
                //slog("rotation: %f,%f,%f", self->rotation.x, self->rotation.y, self->rotation.z);
            }

            if (keys[SDL_SCANCODE_Y])
            {
                self->rotation.x -= 0.02;
                gfc_angle_clamp_radians(&self->rotation.y);
                //slog("rotation: %f,%f,%f", self->rotation.x, self->rotation.y, self->rotation.z);
            }

            //keydown jump
            if (keys[SDL_SCANCODE_SPACE])
            {
                if (!SPACE) {
                    player_move_up(self, 0.1);
                    SPACE = 1;
                }
               
            }
            else {
                if (SPACE) { SPACE = 0; }
            }

            if (keys[SDL_SCANCODE_C])
            {
                player_move_up(self, -moveSpeed);
            }
            
            if (keys[SDL_SCANCODE_U]) {
                slog("ent pos: %f,%f,%f", self->position.x, self->position.y, self->position.z);
                //slog("col pos: %f,%f,%f", self->collider->position.x, self->collider->position.y, self->collider->position.z);
                if (self->collider->primitive.type == GPT_BOX)
                    slog("prim pos: %f,%f,%f", self->collider->primitive.s.b.x, self->collider->primitive.s.b.y, self->collider->primitive.s.b.z);
                if (self->collider->primitive.type == GPT_SPHERE)
                    slog("prim pos: %f,%f,%f", self->collider->primitive.s.s.x, self->collider->primitive.s.s.y, self->collider->primitive.s.s.z);
            }

            if (keys[SDL_SCANCODE_R]) {
                self->alive = 1;
                self->collider->position = gfc_vector3d(0, 0, 0);
                Player_Data* dat = (struct Player_Data*)(self->data);
                dat->health = 100;
            }

            //rotate with movement
            if (self->collider->velocity.x != 0 || self->collider->velocity.y != 0) {
                //self->rotation.z = gfc_vector2d_angle(gfc_vector2d(self->collider->velocity.x, self->collider->velocity.y));
                gfc_angle_clamp_radians(&self->rotation.z);
            }
            cam_orbit(self);
          
            //can this go here?
            cam_follow_player(self, -10);

            return;  
}

void player_draw(Entity* self) {
    //draw self
    if (!self)
        return;
    GFC_Matrix4 matrix;
    GFC_Vector3D corrected;
    gfc_vector3d_set(corrected, self->rotation.y, self->rotation.x, self->rotation.z);
    gfc_matrix4_from_vectors(
       matrix,
       self->position,
       corrected,
       self->scale
   );

    gf3d_model_draw(
        self->model,
        matrix,
        GFC_COLOR_WHITE,
        0
    );
           
    //draw capsule      
   if(!self->capsule || !self->collider) return;
    GFC_Color col, amb;
    GFC_Sphere s1, s2;
    GFC_Edge3D e = get_capsule_endpoints(self->capsule);

    s1 = gfc_sphere(e.a.x, e.a.y, e.a.z, self->capsule->radius);
    s2 = gfc_sphere(e.b.x, e.b.y, e.b.z, self->capsule->radius);
    //slog("capsule: %f %f %f, %f %f %f", pos1.x, pos1.y, pos1.z, pos2.x, pos2.y, pos2.z);
    col = gfc_color(0.2, 0.5, 1, 0.4);
    amb = gfc_color(0, 0, 0, 0);
    gf3d_draw_sphere_solid(s1, gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(1, 1, 1), col, amb);
    gf3d_draw_sphere_solid(s2, gfc_vector3d(0, 0, 0), gfc_vector3d(0, 0, 0), gfc_vector3d(1, 1, 1), col, amb);
}

void player_free(void* data)
{
	if (!data) return;
    slog("freeing player data");
    free(data);
}

void player_move_up(Entity* self, float magnitude)
{
    //should do this for ground jumps otherwise terrain  go boing
    //self->collider->velocity.z= magnitude;

    if (!self || !self->data) return;
    if (!self->collider) {
        slog("player has no collider to move");
        return;
    }

    GFC_Vector3D jump = { 0 };
    GFC_Vector3D invector, forward, up, right;
    gfc_vector3d_set(invector, self->rotation.x, self->rotation.y, self->rotation.z);
    gfc_vector3d_angle_vectors3(invector, &forward, &right, &up);
    if (gfc_vector3d_equal(invector, gfc_vector3d(0, 0, 0)))
        jump = gfc_vector3d_scaled(gfc_vector3d(0, 0, 1), magnitude);
    else
        jump = gfc_vector3d_scaled(right, magnitude);
    slog("rotation: %f,%f,%f", self->rotation.x, self->rotation.y, self->rotation.z);
    slog("jump: %f,%f,%f", up.x, up.y, right.z);
    //negative why??
    //do additive probably better
    self->collider->velocity= jump;
}

void player_walk_forward(Entity* self, float magnitude)
{
    if (!self || !self->data) return;
    if (!self->collider) {
        slog("player has no collider to move");
        return;
    }
    Player_Data* dat = (struct Player_Data*)(self->data);
    GFC_Vector2D w;
    GFC_Vector3D forward = { 0 };
    

    w = gfc_vector2d_from_angle(dat->cam_target.z);
    forward.x = -w.x;
    forward.y = -w.y;
    gfc_vector3d_set_magnitude(&forward, magnitude);
    //if ((temp.x + forward.x) * (temp.x + forward.x) + (temp.y + forward.y) * (temp.y + forward.y) <= maxSpeed * maxSpeed) {
        temp.x += forward.x;
        temp.y += forward.y;
}

void player_walk_right(Entity* self, float magnitude)
{
    if (!self || !self->data) return;
    if (!self->collider) {
        slog("player has no collider to move");
        return;
    }
    Player_Data* dat = (struct Player_Data*)(self->data);
    GFC_Vector2D w;
    GFC_Vector3D right = { 0 };

    w = gfc_vector2d_from_angle(dat->cam_target.z + GFC_HALF_PI);
    right.x = -w.x;
    right.y = -w.y;
    gfc_vector3d_set_magnitude(&right, magnitude);

    //if ((temp.x + right.x) * (temp.x + right.x) + (temp.y + right.y) * (temp.y + right.y) <= maxSpeed * maxSpeed) {
        temp.x += right.x;
        temp.y += right.y;
}
void cam_follow_player(Entity* self, float offset) 
{
    if (!self || !self->data) return;

    GFC_Vector2D w;
    GFC_Vector3D forward = { 0 };
    GFC_Vector3D offsetPos = { 0 };
    Player_Data* dat = (struct Player_Data*)(self->data);

    w = gfc_vector2d_from_angle(dat->cam_target.z);
    forward.x = w.x;
    forward.y = w.y;
    gfc_vector3d_normalize(&forward);
    //slog("befforward: %f,%f,%f", forward.x, forward.y, forward.z);
    gfc_vector3d_scale(forward,forward, offset);
    //slog("aftforward: %f,%f,%f", forward.x, forward.y, forward.z);
   //slog("forward: %f,%f,%f", forward.x, forward.y, forward.z);

    gfc_vector3d_sub(offsetPos, self->position, forward);
    
    gfc_vector3d_copy(forward, self->position);
    forward.z += 1;
    offsetPos.z += 4;
   // slog("position: %f,%f,%f", offsetPos.x, offsetPos.y, offsetPos.z);
    gf3d_camera_look_at(forward, &offsetPos);
    
}

void cam_orbit(Entity* self) {
    if (!self || !self->data) return;
    Player_Data* dat = (struct Player_Data*)(self->data);
    GFC_Vector2D mouse_delta = gf2d_mouse_get_movement();
    
    mouse_delta.x = SDL_clamp(mouse_delta.x, -2, 2); //is this legal
    //slog("moved: %i", gf2d_mouse_moved());
    //slog("orbit: %f,%f", mouse_delta.x, mouse_delta.y);
    dat->cam_target.z -= mouse_delta.x/20;
   // gfc_vector3d_rotate_about_z(&dat->cam_target, mouse_delta.x);
}

void check_world_bounds(Collider* self) {
    int cubic_bound = 30;
    if (!self) return;
    if (self->position.x > cubic_bound || self->position.x < -cubic_bound) self->position = gfc_vector3d(0, 0, 0);
    if (self->position.y > cubic_bound || self->position.y < -cubic_bound) self->position = gfc_vector3d(0, 0, 0);
    if (self->position.z > cubic_bound || self->position.z < -cubic_bound) self->position = gfc_vector3d(0, 0, 0);
}