#include "simple_logger.h"
#include "gfc_matrix.h"
#include "physics.h"


GFC_Vector3D gfc_vector3d_scaled(GFC_Vector3D v, float scalar) {
    GFC_Vector3D scaled;
    gfc_vector3d_set(scaled, v.x * scalar, v.y * scalar, v.z * scalar);
    return scaled;
}

void gfc_vector3d_angle_vectors3(GFC_Vector3D angles, GFC_Vector3D* forward, GFC_Vector3D* right, GFC_Vector3D* up)
{
    float angle;
    float sr, sp, sy, cr, cp, cy;

    angle = angles.z;
    sy = sinf(angle);
    cy = cosf(angle);
    angle = angles.y;
    sp = sinf(angle);
    cp = cosf(angle);
    angle = angles.x;
    sr = sinf(angle);
    cr = cosf(angle);

    if (forward)
    {
        forward->x = (-1 * sr * sp * cy + -1 * cr * -sy);
        forward->y = (-1 * sr * sp * sy + -1 * cr * cy);
        forward->z = -1 * sr * cp;
    }
    if (right)
    {
        right->x = -1 * cp * cy;
        right->y = -1 * cp * sy;
        right->z = sp;
    }
    if (up)
    {
        up->x =  (cr * sp * cy + -sr * -sy);
        up->y =  (cr * sp * sy + -sr * cy);
        up->z =  cr * cp;
    }
}

GFC_Edge3D get_capsule_endpoints(Capsule* self) {
    GFC_Vector3D invector, forward, up, right;
    GFC_Vector3D pos1, pos2, halflength;
    gfc_vector3d_set(invector, self->rotation.x, self->rotation.y, self->rotation.z);
    gfc_vector3d_angle_vectors3(invector, &forward, &right, &up);

    //angle
    if (gfc_vector3d_equal(self->rotation, gfc_vector3d(0, 0, 0)))
        halflength = gfc_vector3d_scaled(gfc_vector3d(0, 1, 0), self->length / 2);
    else
        halflength = gfc_vector3d_scaled(forward, self->length / 2);

    pos1 = gfc_vector3d_added(self->position, halflength);
    pos2 = gfc_vector3d_subbed(self->position, halflength);

    GFC_Edge3D e = { {pos1.x,pos1.y,pos1.z},{pos2.x,pos2.y,pos2.z} };

    return e;
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

GFC_Vector3D ClosestPointOnCapsule(Capsule* capsule, GFC_Triangle3D tri) {
    if (!capsule) return; //this is bad fix later

    GFC_Edge3D endpoints = get_capsule_endpoints(capsule);
    GFC_Vector3D tip, base, t_n, c0, c1, c2;
    // Compute capsule line endpoints A tip, B base like before in capsule-capsule case:
    //i store the axis not the total ends so this is flipped
    GFC_Vector3D CapsuleNormal = gfc_vector3d_subbed(endpoints.a, endpoints.b);
    gfc_vector3d_normalize(&CapsuleNormal);
    GFC_Vector3D LineEndOffset = gfc_vector3d_scaled(CapsuleNormal, capsule->radius);
    tip = gfc_vector3d_added(endpoints.a, LineEndOffset);
    base = gfc_vector3d_subbed(endpoints.b, LineEndOffset);

    // Then for each triangle, ray-plane intersection:
    //  N is the triangle plane normal (it was computed in sphere � triangle intersection case)
     t_n = gfc_trigfc_angle_get_normal(tri);
  //   float distance = gfc_vector3d_dot_product(gfc_vector3d_subbed(center, tri.a), t_n);
    float t = gfc_vector3d_dot_product(t_n, gfc_vector3d_scaled(gfc_vector3d_subbed(tri.a, base), 1 / fabsf(gfc_vector3d_dot_product(t_n, CapsuleNormal))));
    GFC_Vector3D line_plane_intersection = gfc_vector3d_added(base, gfc_vector3d_scaled(CapsuleNormal, t));

    GFC_Vector3D reference_point;

    //The �find closest point on triangle to line_plane_intersection� refers to the same approach that was used in the sphere � triangle intersection case, when we try to determine if the sphere centre projection is inside the triangle, or find a closest edge point.Here, instead of the sphere centre projection, we use the line_plane_intersection point :

    // Determine whether line_plane_intersection is inside all triangle edges: 
    gfc_vector3d_cross_product(&c0, gfc_vector3d_subbed(line_plane_intersection, tri.a), gfc_vector3d_subbed(tri.b, tri.a));
    gfc_vector3d_cross_product(&c1, gfc_vector3d_subbed(line_plane_intersection, tri.a), gfc_vector3d_subbed(tri.b, tri.a));
    gfc_vector3d_cross_product(&c2, gfc_vector3d_subbed(line_plane_intersection, tri.a), gfc_vector3d_subbed(tri.b, tri.a));

    Uint8 inside = gfc_vector3d_dot_product(c0, t_n) <= 0 && gfc_vector3d_dot_product(c1, t_n) <= 0 && gfc_vector3d_dot_product(c2, t_n) <= 0;

    if (inside)
    {
        reference_point = line_plane_intersection;
    }
    else
    {
        // Edge 1:
        GFC_Vector3D point1 = ClosestPointOnLineSegment(tri.a, tri.b, line_plane_intersection);
        GFC_Vector3D v1 = gfc_vector3d_subbed(line_plane_intersection, point1);
        float distsq = gfc_vector3d_dot_product(v1, v1);
        float best_dist = distsq;
        reference_point = point1;

        // Edge 2:
        GFC_Vector3D point2 = ClosestPointOnLineSegment(tri.b, tri.c, line_plane_intersection);
        GFC_Vector3D v2 = gfc_vector3d_subbed(line_plane_intersection, point2);
        distsq = gfc_vector3d_dot_product(v2, v2);
        if (distsq < best_dist)
        {
            reference_point = point2;
            best_dist = distsq;
        }

        // Edge 3:
        GFC_Vector3D point3 = ClosestPointOnLineSegment(tri.c, tri.a, line_plane_intersection);
        GFC_Vector3D v3 = gfc_vector3d_subbed(line_plane_intersection, point3);
        distsq = gfc_vector3d_dot_product(v3, v3);
        if (distsq < best_dist)
        {
            reference_point = point3;
            best_dist = distsq;
        }
    }


    // The center of the best sphere candidate:
    GFC_Vector3D center = ClosestPointOnLineSegment(endpoints.a, endpoints.b, reference_point);
    return center;
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
   //slog("colliding");
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

Uint8 capsule_to_mesh_collision(Capsule* cap, ObjData* obj) {
    if (!cap || !obj) return false;
    int i;
    Uint8 collided = false;
    GFC_Vector3D pen_norm = gfc_vector3d(0, 0, 0);
    float pen_depth = 0;
    for (i = 0; i < obj->face_count; i++)
    {
        GFC_Vector3D p0, p1, p2, closest;
        p0 = obj->vertices[obj->faceVerts[i].verts[0]];
        p1 = obj->vertices[obj->faceVerts[i].verts[1]];
        p2 = obj->vertices[obj->faceVerts[i].verts[2]];
        GFC_Triangle3D tri = gfc_triangle(p0, p1, p2);
        //slog("triangle %d: p0: %f, %f, %f  p1: %f, %f, %f p2: %f, %f, %f", i, p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
        //curently stops on first colliding triangle
        closest = ClosestPointOnCapsule(cap, tri);
        GFC_Sphere sphere = gfc_sphere(closest.x, closest.y, closest.z, cap->radius);
        if (sphere_to_triangle_collision(sphere, tri, &pen_norm, &pen_depth)) {
           // sphere_to_triangle_resolution(col, tri, pen_norm, pen_depth);
           slog("norm: %f, %f, %f  depth: %f", pen_norm.x, pen_norm.y, pen_norm.z, pen_depth);
            collided = true;
            slog("collided");
        }
    }
    return collided;
}