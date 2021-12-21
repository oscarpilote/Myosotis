#pragma once

#include <stdint.h>

#include "vec3.h"
#include "aabb.h"
#include "mesh.h"
#include "vertex_table.h"

Aabb compute_mesh_bounds(const Vec3* positions, size_t vertex_count);

Aabb compute_mesh_bounds(const Mesh& mesh, const MeshData& data);

void compute_mesh_normals(const Mesh& mesh, MeshData& data);

void concat_mesh(Mesh& dst_m, MeshData& dst_d, const Mesh& src_m, 
		 const MeshData& src_d);

void join_mesh(Mesh& dst_m, MeshData& dst_d, const Mesh& src_m, 
		const MeshData& src_d, VertexTable* vtx_table);

void compact_mesh(Mesh& mesh, MeshData& data, uint32_t *remap);

void copy_indices(MeshData& dst, size_t dst_off, const MeshData& src, 
		  size_t src_off, size_t idx_num, size_t vtx_off = 0);

void copy_vertices(MeshData& dst, size_t dst_off, const MeshData& src,
		   size_t src_off, size_t vtx_num, size_t vtx_off = 0);

