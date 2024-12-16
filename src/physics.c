#include "simple_logger.h"
#include "gfc_matrix.h"
#include "physics.h"

GFC_Vector3D gfc_vector3d_scaled(GFC_Vector3D v, float scalar) {
    GFC_Vector3D scaled;
    gfc_vector3d_set(scaled, v.x * scalar, v.y * scalar, v.z * scalar);
    return scaled;
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
Mesh_Collider* mesh_collider_new(ObjData* obj) {
        int j;   
        Mesh_Collider* mc = malloc(sizeof(Mesh_Collider));
        if (!mc) {
            slog("could not allocate mesh collider");
            return NULL;
        }

        mc->triangle_verts = obj->faceVerts;
        mc->triangle_normals = obj->faceNormals;
        mc->tri_count = obj->face_count;
        //slog("has %d", obj->vertex_count);
        return mc;   
}

GFC_Vector3D ClosestPointOnLineSegment(GFC_Vector3D a, GFC_Vector3D b, GFC_Vector3D point)
{
    GFC_Vector3D ab = gfc_vector3d_subbed(b,a);
    float t = gfc_vector3d_dot_product(gfc_vector3d_subbed(point, a), ab) / gfc_vector3d_dot_product(ab, ab);
    float saturate = min(max(t, 0), 1);
    return gfc_vector3d_added(b, gfc_vector3d_scaled(ab, saturate)); 
}

//maybe pass by pointer
Uint8 sphere_to_triangle_collision(GFC_Sphere s, GFC_Triangle3D t) {
    GFC_Vector3D t_n, center, projected_point, c0, c1, c2;
    float distance;

    center = gfc_vector3d(s.x, s.y, s.z);
    t_n = gfc_trigfc_angle_get_normal(t);

    distance = gfc_vector3d_dot_product(gfc_vector3d_subbed(center, t.a), t_n);

    //something about double sided don't think I need?

    //intersection with triangle plane test
    if (distance < -s.r || distance > s.r)
        return false;// no intersection

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
   GFC_Vector3D penetration_normal = gfc_vector3d_scaled(intersection_vec, 1 / len);  // normalize, probably quicker than the function?
   float penetration_depth = s.r - len; 
   slog("colliding");
   return true; // intersection success
    
}

Uint8 sphere_to_mesh_collision(GFC_Sphere sphere, ObjData* obj) {
    int i;
    for (i = 0; i < obj->face_count; i++)
    {
        GFC_Vector3D p0, p1, p2;
        p0 = obj->vertices[obj->faceVerts[i].verts[0]];
        p1 = obj->vertices[obj->faceVerts[i].verts[1]];
        p2 = obj->vertices[obj->faceVerts[i].verts[2]];
        GFC_Triangle3D tri = gfc_triangle(p0, p1, p2);
        //slog("triangle %d: p0: %f, %f, %f  p1: %f, %f, %f p2: %f, %f, %f", i, p0.x, p0.y, p0.z, p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
        //curently stops on first colliding triangle
        return sphere_to_triangle_collision(sphere, tri);
    }

}
void mesh_collider_free(Mesh_Collider* mc) {
    free(mc);
    
}