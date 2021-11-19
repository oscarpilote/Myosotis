#include "array.h"
#include "vec3.h"
#include "vec2.h"

struct Mesh {
	TArray<unsigned> indices;
	TArray<Vec3> positions;
	TArray<Vec3> normals;
	TArray<Vec2> texcoords;
};

Mesh obj_to_mesh(const char* filename);

