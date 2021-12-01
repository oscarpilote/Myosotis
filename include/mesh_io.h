#include <stdio.h>
#include "fast_obj/fast_obj.h"
#include "mesh.h"

int obj_to_mesh(const fastObjMesh& obj, MeshData& data, Mesh& mesh);
int ply_to_mesh(const char* filename, MeshData &data, Mesh &mesh);

