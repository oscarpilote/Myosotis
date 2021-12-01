#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "mesh.h"
#include "vec3.h"
#include "vec2.h"

void MeshData::clear()
{
	free(indices);
	idx_capacity = 0;
	free(positions);
	free(normals);
	for (int i = 0; i < MAX_UV_MAPS; ++i)
	{
		free(uv[i]);
	}
	vtx_capacity = 0;
}

void MeshData::reserve_indices(size_t count, bool shrink)
{
	assert(count > 0);

	if ((count <= idx_capacity) && (!shrink))
	{
		return;
	}
	
	indices = (uint32_t *)realloc(indices, count * sizeof(uint32_t));
	idx_capacity = count;
}

void MeshData::reserve_vertices(size_t count, bool shrink)
{
	assert(count > 0);

	if ((count <= vtx_capacity) && (!shrink))
	{
		return;
	}

	positions = (Vec3 *)realloc(positions, count * sizeof(Vec3));
	if (vtx_attribs & VertexAttrib::NML)
	{
		normals = (Vec3 *)realloc(normals, count * sizeof(Vec3));
	}
	if (vtx_attribs & VertexAttrib::UV0)
	{
		uv[0] = (Vec2 *)realloc(uv[0], count * sizeof(Vec2));
	}
	if (vtx_attribs & VertexAttrib::UV1)
	{
		uv[1] = (Vec2 *)realloc(uv[1], count * sizeof(Vec2));
	}
	vtx_capacity = count;
}

