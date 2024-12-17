#include "simple_logger.h"
#include "gfc_matrix.h"
#include "physics.h"


GFC_Vector3D gfc_vector3d_scaled(GFC_Vector3D v, float scalar) {
    GFC_Vector3D scaled;
    gfc_vector3d_set(scaled, v.x * scalar, v.y * scalar, v.z * scalar);
    return scaled;
}

void gfc_vector3d_local_angle_vectors(GFC_Vector3D angles, GFC_Vector3D* forward, GFC_Vector3D* right, GFC_Vector3D* up)
{
    if (forward)
    {
        forward->x = 0;
        forward->y = 1;
        forward->z = 0;
        gfc_vector3d_rotate_about_x(forward, angles.x);
        gfc_vector3d_rotate_about_z(forward, angles.z);
        gfc_vector3d_rotate_about_y(forward, angles.y);
    }
    if (right)
    {
        right->x = 1;
        right->y = 0;
        right->z = 0;
        gfc_vector3d_rotate_about_y(right, angles.y);
        gfc_vector3d_rotate_about_z(right, angles.z);
        gfc_vector3d_rotate_about_x(right, angles.x);
    }
    if (up)
    {
        up->x = 0;
        up->y = 0;
        up->z = 1;
        gfc_vector3d_rotate_about_y(up, angles.y);
        gfc_vector3d_rotate_about_x(up, angles.x);
        gfc_vector3d_rotate_about_z(up, angles.z);
    }
}
//switched up and forward may need to undo later if dependent on model rotation separate from entity??? idek
GFC_Vector3D vector3d_forward(GFC_Vector3D rotation) {
    GFC_Vector3D forward; 
    //x pitch y roll z yaw
    forward.x = cos(rotation.x) * sin(rotation.z);
    forward.z = -sin(rotation.x);
    forward.y = cos(rotation.x) * cos(rotation.z);
    gfc_angle_clamp_radians(&forward.x);
    gfc_angle_clamp_radians(&forward.y);
    gfc_angle_clamp_radians(&forward.z);
    return forward;
}
GFC_Vector3D vector3d_right(GFC_Vector3D rotation) {
    GFC_Vector3D  right;
    
    right.x = cos(rotation.z);
    right.z = 0;
    right.y = -sin(rotation.z);
    
    
    
    return right;
}
GFC_Vector3D vector3d_up(GFC_Vector3D rotation) {
    GFC_Vector3D mat_rotation, up, world_up, out;
    GFC_Matrix4 mat;
    world_up = gfc_vector3d(0, 0, 1);
   /*up.x = sin(rotation.x) * sin(rotation.z);
   // up.z = cos(rotation.x);
   // up.y = sin(rotation.x) * cos(rotation.z);

    gfc_matrix4_from_vectors(mat, gfc_vector3d(0, 0, 0), rotation, gfc_vector3d(1, 1, 1));
    gfc_matrix4_to_vectors(mat, NULL, &mat_rotation, NULL);
    slog("matrix rotation: %f,%f,%f", mat_rotation.x, mat_rotation.y, mat_rotation.z);
    up.x = -sinf(mat_rotation.x) * sinf(mat_rotation.z);
    up.z = cosf(mat_rotation.x);
    up.y = -sinf(mat_rotation.x) * cosf(mat_rotation.z);
    */
    slog("rotation: %f,%f,%f", rotation.x, rotation.y, rotation.z);
    up = gfc_vector3d_multiply(rotation, world_up);
    //gfc_vector3d_angle_vectors(up, &out.x)

    return up;
}

Uint8 gfc_vector3d_equal(GFC_Vector3D a, GFC_Vector3D b) {
    if (a.x != b.x)
        return 0;
    if (a.y != b.y)
        return 0;
    if (a.z != b.z)
        return 0;
    return 1;
}
Capsule* capsule_new(GFC_Vector3D position, GFC_Vector3D rotation, float length, float radius) {
        int j;   
        Capsule* cap = malloc(sizeof(Capsule));
        if (!cap) {
            slog("could not allocate capsule");
            return NULL;
        }

        cap->position = position;
        cap->rotation = rotation;
        cap->length = length;
        cap->radius = radius;
        return cap;   
}

GFC_Vector3D ClosestPointOnLineSegment(GFC_Vector3D a, GFC_Vector3D b, GFC_Vector3D point)
{
    GFC_Vector3D ab = gfc_vector3d_subbed(b,a);
    float t = gfc_vector3d_dot_product(gfc_vector3d_subbed(point, a), ab) / gfc_vector3d_dot_product(ab, ab);
    float saturate = min(max(t, 0), 1);
    return gfc_vector3d_added(a, gfc_vector3d_scaled(ab, saturate)); 
}

//maybe pass by pointer
Uint8 sphere_to_triangle_collision(GFC_Sphere s, GFC_Triangle3D t, GFC_Vector3D* pen_norm, float* pen_depth) {
    GFC_Vector3D t_n, center, projected_point, c0, c1, c2;
    float distance;
    //slog("triangle: %f %f %f, %f %f %f, %f %f %f", t.a.x, t.a.y, t.a.z, t.b.x, t.b.y, t.b.z, t.c.x, t.c.y, t.c.z);
    center = gfc_vector3d(s.x, s.y, s.z);
    //slog("center: %f %f %f", center.x, center.y, center.z);
    
    t_n = gfc_trigfc_angle_get_normal(t);
    //slog("normal: %f %f %f,", t_n.x, t_n.y, t_n.z);
    distance = gfc_vector3d_dot_product(gfc_vector3d_subbed(center, t.a), t_n);

    //something about double sided don't think I need?
    //slog("distance: %f, radius: %f", distance, s.r);
    //intersection with triangle plane test
    if (distance < -s.r || distance > s.r)
        return false;// no intersection
    else
    {
        //slog("collided");
        //return true;
    }

    projected_point = gfc_vector3d_subbed(center, gfc_vector3d_scaled(t_n, distance)); // projected sphere center on triangle plane
    
    // Now determine whether the projected point is inside all triangle edges: 
    gfc_vector3d_cross_product(&c0, gfc_vector3d_subbed(projected_point, t.a), gfc_vector3d_subbed(t.b, t.a));
    gfc_vector3d_cross_product(&c1, gfc_vector3d_subbed(projected_point, t.b), gfc_vector3d_subbed(t.c, t.b));
    gfc_vector3d_cross_product(&c2, gfc_vector3d_subbed(projected_point, t.c), gfc_vector3d_subbed(t.a, t.c));

    //if true, projected point is closest point on triangle to the sphere center
    Uint8 inside = gfc_vector3d_dot_product(c0, t_n) <= 0 && gfc_vector3d_dot_product(c1, t_n) <= 0 && gfc_vector3d_dot_product(c2, t_n) <= 0;

    float rsq = s.r * s.r; // sphere radius squared

    // Edge 1:
    GFC_Vector3D point1 = ClosestPointOnLineSegment(t.a, t.b, center);
    GFC_Vector3D v1 = gfc_vector3d_subbed(center, point1);
    float distsq1 = gfc_vector3d_dot_product(v1, v1);
    Uint8 intersects = (distsq1 < rsq);

    // Edge 2:
    GFC_Vector3D point2 = ClosestPointOnLineSegment(t.b, t.c, center);
    GFC_Vector3D v2 = gfc_vector3d_subbed(center, point2);
    float distsq2 = gfc_vector3d_dot_product(v2, v2);
    intersects = intersects || distsq2 < rsq;

    // Edge 3:
    GFC_Vector3D point3 = ClosestPointOnLineSegment(t.c, t.a, center);
    GFC_Vector3D v3 = gfc_vector3d_subbed(center, point3);
    float distsq3 = gfc_vector3d_dot_product(v3, v3);
    intersects = intersects || distsq3 < rsq;

    //intersects is final check on if we are colliding
    if (!inside && !intersects)
        return false;
    
    GFC_Vector3D best_point = projected_point;
    GFC_Vector3D intersection_vec;

   if (inside)
   {
       intersection_vec = gfc_vector3d_subbed(center, projected_point);
   }
   else
   {
       GFC_Vector3D d = gfc_vector3d_subbed(center, point1);
       float best_distsq = gfc_vector3d_dot_product(d, d);
       best_point = point1;
       intersection_vec = d;

       d = gfc_vector3d_subbed(center, point2);
       float distsq = gfc_vector3d_dot_product(d, d);
       if (distsq < best_distsq)
       {
           distsq = best_distsq;
           best_point = point2;
           intersection_vec = d;
       }

       d = gfc_vector3d_subbed(center, point3);
       //changed from new float should be fine
       distsq = gfc_vector3d_dot_product(d, d);
       if (distsq < best_distsq)
       {
           distsq = best_distsq;
           best_point = point3;
           intersection_vec = d;
       }
   }
   
   //seemed wrong type on tutorial, should be float?
   
   float len = gfc_vector3d_magnitude(intersection_vec); 
   *pen_norm = gfc_vector3d_scaled(intersection_vec, 1 / len);  // normalize, probably quicker than the function?
   *pen_depth = s.r - len;
   slog("colliding");
   return true; // intersection success
    
}

void sphere_to_triangle_resolution(Collider* col, GFC_Triangle3D t, GFC_Vector3D normal, float depth) {
    if (!col) return;
    
        float velocity_length = gfc_vector3d_magnitude(col->velocity);
    if (velocity_length != 0) {
        GFC_Vector3D velocity_normalized = gfc_vector3d_scaled(col->velocity, 1 / velocity_length);
        GFC_Vector3D undesired_motion = gfc_vector3d_scaled(normal, gfc_vector3d_dot_product(velocity_normalized, normal));
        GFC_Vector3D desired_motion = gfc_vector3d_subbed(velocity_normalized, undesired_motion);
        col->velocity = gfc_vector3d_scaled(desired_motion, velocity_length);
    }
    // Remove penetration (penetration epsilon added to handle infinitely small penetration):
    col->position = gfc_vector3d_added(col->position, gfc_vector3d_scaled(normal, (depth + 0.0001)));

}

Uint8 sphere_to_mesh_collision(Collider* col, ObjData* obj) {
    if (!col || !obj) return false;
    int i;
    Uint8 collided = false;
    GFC_Vector3D pen_norm = gfc_vector3d(0,0,0);
    float pen_depth = 0;
    GFC_Sphere sphere = col->primitive.s.s;
    for (i = 0; i < obj->face_count; i++)
    {
        GFC_Vector3D p0, p1, p2;
        p0 = obj->vertices[obj->faceVerts[i].verts[0]];
        p1 = obj->vertices[obj->faceVerts[i].verts[1]];
        p2 = obj->vertices[obj->faceVerts[i].verts[2]];
        GFC_Triangle3D tri = gfc_triangle(p0, p1, p2);
        //slog("triangle %d: p0: %f, %f, %f  p1: %f, %f, %f p2: %f, %f, %f", i, p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
        //curently stops on first colliding triangle
        if (sphere_to_triangle_collision(sphere, tri, &pen_norm, &pen_depth)) {
            sphere_to_triangle_resolution(col, tri, pen_norm, pen_depth);
            slog("norm: %f, %f, %f  depth: %f", pen_norm.x, pen_norm.y, pen_norm.z, pen_depth);
            collided = true;
        }
    }
    return collided;
}