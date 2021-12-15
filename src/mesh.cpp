#include "mesh.h"

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "vec3.h"
#include "vec2.h"
#include "sys_utils.h"

void MeshData::clear()
{
	MEMFREE(indices);
	idx_capacity = 0;
	MEMFREE(positions);
	MEMFREE(normals);
	MEMFREE(uv[0]);
	MEMFREE(uv[1]);
	MEMFREE(parents);
	vtx_capacity = 0;
}

void MeshData::reserve_indices(size_t num, bool shrink)
{
	assert(num > 0);

	if ((num <= idx_capacity) && (!shrink))
	{
		return;
	}
	
	REALLOC_NUM(indices, num);
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
		REALLOC_NUM(positions, num);
	}
	
	if (vtx_attribs & VertexAttrib::NML)
	{
		REALLOC_NUM(normals, num);
	}
	
	if (vtx_attribs & VertexAttrib::UV0)
	{
		REALLOC_NUM(uv[0], num);
	}
	
	if (vtx_attribs & VertexAttrib::UV1)
	{
		REALLOC_NUM(uv[1], num);
	}
	
	if (vtx_attribs & VertexAttrib::PAR)
	{
		REALLOC_NUM(parents, num);
	}
	
	vtx_capacity = num;
}

void Mesh::clear()
{
	data.clear();
	MEMFREE(patches);
	num_patches = 0;
}

void Mesh::reserve_patches(size_t num, bool shrink)
{
	assert(num > 0);

	if ((num > num_patches) || (num < num_patches && shrink))
	{
		REALLOC_NUM(patches, num);
	}
}

