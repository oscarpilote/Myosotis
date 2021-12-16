#include "mesh_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "vec3.h"
#include "aabb.h"
#include "mesh.h"
#include "array.h"
#include "geometry.h"
#include "mesh_remap.h"

Aabb compute_mesh_bounds(const Vec3* positions, size_t vertex_count)
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

Aabb compute_mesh_bounds(const Mesh& mesh, const MeshData& data)
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

void copy_indices(MeshData& dst, size_t dst_off, const MeshData& src, 
		  size_t src_off, size_t idx_num, size_t vtx_off)
{
	static_assert(
		sizeof(*dst.indices) == sizeof(*src.indices), 
		"Error in copy_vertices: dst and src indices type mismatch."
		);
	
	assert(src.idx_capacity >= src_off + idx_num);
	assert(dst.idx_capacity >= dst_off + idx_num);

	void *to;
	void *from;

	to   = &dst.indices[dst_off]; 
	from = &src.indices[src_off]; 
	memcpy(to, from, idx_num * sizeof(*src.indices));

	if (vtx_off)
	{
		for (size_t i = 0; i < idx_num; ++i)
		{
			dst.indices[dst_off + i] += vtx_off;
		}
	}
}

void copy_vertices(MeshData& dst, size_t dst_off, const MeshData& src,
		   size_t src_off, size_t vtx_num, size_t vtx_off)
{
	assert((src.vtx_attribs & dst.vtx_attribs) == dst.vtx_attribs);
	
	assert(src.vtx_capacity >= src_off + vtx_num);
	assert(dst.vtx_capacity >= dst_off + vtx_num);

	void *to;
	void *from;

	to   = &dst.positions[dst_off]; 
	from = &src.positions[src_off]; 
	memcpy(to, from, vtx_num * sizeof(*src.positions));

	if (dst.vtx_attribs & VertexAttrib::NML)
	{
		to   = &dst.normals[dst_off]; 
		from = &src.normals[src_off]; 
		memmove(to, from, vtx_num * sizeof(*src.normals));
	}

	if (dst.vtx_attribs & VertexAttrib::UV0)
	{
		to   = &dst.uv[0][dst_off]; 
		from = &src.uv[0][src_off]; 
		memmove(to, from, vtx_num * sizeof(*src.uv[0]));
	}
	
	if (dst.vtx_attribs & VertexAttrib::UV1)
	{
		to   = &dst.uv[1][dst_off]; 
		from = &src.uv[1][src_off]; 
		memmove(to, from, vtx_num * sizeof(*src.uv[1]));
	}
	
	if (dst.vtx_attribs & VertexAttrib::PAR)
	{
		to   = &dst.parents[dst_off]; 
		from = &src.parents[src_off]; 
		memmove(to, from, vtx_num * sizeof(*src.parents));
		if (vtx_off)
		{
			for (size_t i = 0; i < vtx_num; ++i)
			{
				dst.parents[dst_off + i] += vtx_off;
			}
		}
	}
}

void concat_meshes(const Mesh* meshes, size_t num_mesh, const MeshData& src, 
		  Mesh& group, MeshData& dst)
{
	dst.vtx_attribs = src.vtx_attribs;
	
	group.index_offset  = 0;
	group.vertex_offset = 0;

	size_t total_indices  = 0;
	size_t total_vertices = 0;
	
	for (size_t i = 0; i < num_mesh; ++i)
	{
		total_indices  += meshes[i].index_count;
		total_vertices += meshes[i].vertex_count;
	}
	
	dst.reserve_indices(total_indices);
	group.index_count = total_indices;
	
	dst.reserve_vertices(total_vertices);
	group.vertex_count = total_vertices;

	total_indices = 0;
	total_vertices = 0;
	for (size_t i = 0; i < num_mesh; ++i)
	{
		size_t dst_off, src_off, idx_num, vtx_num, vtx_off;

		src_off = meshes[i].index_offset;
		dst_off = total_indices;
		idx_num = meshes[i].index_count;
		vtx_off = total_vertices;
		copy_indices(dst, dst_off, src, src_off, idx_num, vtx_off);

		src_off = meshes[i].vertex_offset;
		dst_off = total_vertices;
		vtx_num = meshes[i].vertex_count;
		vtx_off = total_vertices;
		copy_vertices(dst, dst_off, src, src_off, vtx_num, vtx_off);
		
		total_indices  += meshes[i].index_count;
		total_vertices += meshes[i].vertex_count;
	}
}

