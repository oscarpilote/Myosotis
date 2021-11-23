#include <stdio.h>

#include "array.h"
#include "vec3.h"
#include "vec2.h"

/*template<typename T>
struct Stream {
	T*  data   = nullptr;
	size_t stride = 0;
	size_t num    = 0;
};

template<typename T>
struct GMesh {
	Stream indices;
	Stream positions;
	Stream normals;
	Stream texcoords;
};*/

struct Mesh {
	TArray<unsigned> indices;
	TArray<Vec3> positions;
	TArray<Vec3> normals;
	TArray<Vec2> texcoords;
	Mesh() = default;
	Mesh(const Mesh& m) = default;
};

Mesh obj_to_mesh(const char* filename);

