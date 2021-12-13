#pragma once

#include <stdint.h>

#include "vec3.h"
#include "bbox.h"
#include "mesh.h"

Bbox compute_mesh_bounds(const Vec3* positions, size_t vertex_count);

Bbox compute_mesh_bounds(const Mesh& mesh, const MeshData& data);

void compute_mesh_normals(const Mesh& mesh, MeshData& data);

void copy_indices(MeshData& dst, size_t idx_d, const MeshData& src, 
		  size_t idx_s, size_t num, size_t vtx_offset);

void copy_vertices(MeshData& dst, size_t idx_d, const MeshData& src,
		   size_t idx_s, size_t num);

void group_meshes(const Mesh* meshes, size_t nmesh, const MeshData& src, 
		  Mesh& group, MeshData& dst);

