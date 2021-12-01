#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "mesh_utils.h"
#include "vec3.h"
#include "bbox.h"
#include "mesh.h"
#include "array.h"
#include "geometry.h"
#include "mesh_remap.h"

Bbox compute_mesh_bounds(const Vec3* positions, size_t vertex_count)
{
	Vec3 min = positions[0];
	Vec3 max = positions[0];
	
	for (size_t i = 1; i < vertex_count; ++i)
	{
		const Vec3& pos = positions[i];

		for (size_t j = 0; j < 3; ++j)
		{
			min[j] = (pos[j] < min[j]) ? pos[j] : min[j];
			max[j] = (pos[j] > max[j]) ? pos[j] : max[j];
		}
	}

	return {min, max}; 
}

Bbox compute_mesh_bounds(const Mesh& mesh, const MeshData& data)
{
	const Vec3 *positions = data.positions + mesh.vertex_offset;
	size_t vertex_count = mesh.vertex_count;

	return (compute_mesh_bounds(positions, vertex_count));
}

void compute_mesh_normals(const Mesh& mesh, MeshData& data)
{
	if (!(data.vtx_attribs & VertexAttrib::NML))
	{
		
		void *normals = malloc(data.vtx_capacity * sizeof(Vec3));
		data.normals = static_cast<Vec3*>(normals);
		data.vtx_attribs |= VertexAttrib::NML;

	}

	TArray<uint32_t> remap(mesh.vertex_count);
	build_position_remap(mesh, data, &remap[0]);

	uint32_t *indices = data.indices + mesh.index_offset;
	const Vec3* positions = data.positions + mesh.vertex_offset; 
	Vec3* normals = data.normals + mesh.vertex_offset; 
	/* Init normals to zero */
	for (size_t i = 0; i < mesh.vertex_count; ++i)
	{
		normals[i] = Vec3::Zero;
	}
	for (size_t i = 0; i < mesh.index_count; i+=3)
	{
		const Vec3 v1 = positions[indices[i + 0]];
		const Vec3 v2 = positions[indices[i + 1]];
		const Vec3 v3 = positions[indices[i + 2]];

		/* Weight normals by triangle area */
		Vec3 n = cross(v2 - v1, v3 - v1);
		
		/* Accumulate normals of remap targets */
		normals[remap[indices[i + 0]]] += n;
		normals[remap[indices[i + 1]]] += n;
		normals[remap[indices[i + 2]]] += n;
	}

	/* Normalize remap targets and copy them to remap sources */
	for (size_t i = 0; i < mesh.vertex_count; ++i)
	{
		if (remap[i] == i)
		{
			normals[i] = normalized(normals[i]);
		}
		else
		{
			assert(remap[i] < i);
			normals[i] = normals[remap[i]];
		}
	}
}

void copy_vertices(MeshData& dst, size_t idx_d, const MeshData& src,
		size_t idx_s, size_t num)
{
	assert((src.vtx_attribs & dst.vtx_attribs) == dst.vtx_attribs);
	
	assert(src.vtx_capacity >= idx_s + num);
	assert(dst.vtx_capacity >= idx_d + num);

	void *to;
	void *from;

	to   = &dst.positions[idx_d]; 
	from = &src.positions[idx_s]; 
	memcpy(to, from, num * sizeof(Vec3));

	if (dst.vtx_attribs & VertexAttrib::NML)
	{
		to   = &dst.normals[idx_d]; 
		from = &src.normals[idx_s]; 
		memmove(to, from, num * sizeof(Vec3));
	}

	if (dst.vtx_attribs & VertexAttrib::UV0)
	{
		to   = &dst.uv[0][idx_d]; 
		from = &src.uv[0][idx_s]; 
		memmove(to, from, num * sizeof(Vec2));
	}
	
	if (dst.vtx_attribs & VertexAttrib::UV1)
	{
		to   = &dst.uv[1][idx_d]; 
		from = &src.uv[1][idx_s]; 
		memmove(to, from, num * sizeof(Vec2));
	}
}

