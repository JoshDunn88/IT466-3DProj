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

        mc->triangle_verts = obj->faceVerts;
        mc->triangle_normals = obj->faceNormals;
        mc->tri_count = obj->face_count;
        //slog("has %d", obj->vertex_count);
        return mc;   
}

Uint8 sphere_to_mesh_collision(GFC_Sphere sphere, Mesh_Collider* mc) {

}


void mesh_collider_free(Mesh_Collider* mc) {
    free(mc);
    
}