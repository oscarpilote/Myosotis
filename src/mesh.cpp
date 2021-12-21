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
	
	if (vtx_attr & VtxAttr::NML)
	{
		REALLOC_NUM(normals, num);
	}
	
	if (vtx_attr & VtxAttr::UV0)
	{
		REALLOC_NUM(uv[0], num);
	}
	
	if (vtx_attr & VtxAttr::UV1)
	{
		REALLOC_NUM(uv[1], num);
	}
	
	if (vtx_attr & VtxAttr::PAR)
	{
		REALLOC_NUM(parents, num);
	}
	
	vtx_capacity = num;
}
