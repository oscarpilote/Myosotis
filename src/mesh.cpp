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
	free(uv[0]);
	free(uv[1]);
	free(parents);
	vtx_capacity = 0;
}

void MeshData::reserve_indices(size_t num, bool shrink)
{
	assert(num > 0);

	if ((num <= idx_capacity) && (!shrink))
	{
		return;
	}
	
	indices = (uint32_t *)realloc(indices, num * sizeof(uint32_t));
	idx_capacity = num;
}

void MeshData::reserve_vertices(size_t num, bool shrink)
{
	assert(num > 0);

	if ((num <= vtx_capacity) && (!shrink))
	{
		return;
	}

	if (true)
	{
		positions = (Vec3 *)realloc(positions, num * sizeof(Vec3));
	}
	
	if (vtx_attribs & VertexAttrib::NML)
	{
		normals = (Vec3 *)realloc(normals, num * sizeof(Vec3));
	}
	
	if (vtx_attribs & VertexAttrib::UV0)
	{
		uv[0] = (Vec2 *)realloc(uv[0], num * sizeof(Vec2));
	}
	
	if (vtx_attribs & VertexAttrib::UV1)
	{
		uv[1] = (Vec2 *)realloc(uv[1], num * sizeof(Vec2));
	}
	
	if (vtx_attribs & VertexAttrib::PAR)
	{
		parents = (uint32_t *)realloc(parents, num * sizeof(uint32_t));
	}
	
	vtx_capacity = num;
}

