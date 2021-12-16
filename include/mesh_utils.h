#pragma once

#include <stdint.h>

#include "vec3.h"
#include "aabb.h"
#include "mesh.h"

Aabb compute_mesh_bounds(const Vec3* positions, size_t vertex_count);

Aabb compute_mesh_bounds(const Mesh& mesh, const MeshData& data);

void compute_mesh_normals(const Mesh& mesh, MeshData& data);

void copy_indices(MeshData& dst, size_t dst_off, const MeshData& src, 
		  size_t src_off, size_t idx_num, size_t vtx_off = 0);

void copy_vertices(MeshData& dst, size_t dst_off, const MeshData& src,
		   size_t src_off, size_t vtx_num, size_t vtx_off = 0);

void concat_meshes(const Mesh* meshes, size_t num_mesh, const MeshData& src, 
		  Mesh& group, MeshData& dst);

