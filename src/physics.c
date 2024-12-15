#include "simple_logger.h"
#include "gfc_matrix.h"
#include "physics.h"

Mesh_Collider* mesh_collider_new(ObjData* obj) {
        int j;   
        Mesh_Collider* mc = malloc(sizeof(Mesh_Collider));
        if (!mc) {
            slog("could not allocate mesh collider");
            return NULL;
        }

        mc->triangles = gfc_allocate_array(sizeof(GFC_Triangle3D), obj->face_count);
        mc->normals = gfc_allocate_array(sizeof(GFC_Vector3D), obj->face_count);
        
        for (j = 0; j < obj->face_count; j++) {
           // slog("vert #%d: %f, %f, %f ", j, obj->vertices[j].x, obj->vertices[j].y, obj->vertices[j].z);
            
            mc->triangles[j].a = gfc_vector3d(obj->vertices[obj->faceVerts->verts[0]].x,
                                              obj->vertices[obj->faceVerts->verts[0]].y,
                                              obj->vertices[obj->faceVerts->verts[0]].z);
            mc->triangles[j].b = gfc_vector3d(obj->vertices[obj->faceVerts->verts[1]].x,
                                                obj->vertices[obj->faceVerts->verts[1]].y,
                                                obj->vertices[obj->faceVerts->verts[1]].z);
            mc->triangles[j].c = gfc_vector3d(obj->vertices[obj->faceVerts->verts[2]].x,
                                                obj->vertices[obj->faceVerts->verts[2]].y,
                                                obj->vertices[obj->faceVerts->verts[2]].z);

            mc->normals[j] = gfc_trigfc_angle_get_normal(mc->triangles[j]);
        }


    
}

void mesh_collider_free(Mesh_Collider* mc) {

    free(mc->triangles);
    free(mc->normals);
    free(mc);
    
}