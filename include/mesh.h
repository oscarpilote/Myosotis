#pragma once

#include <stdint.h>

#include "vec2.h"
#include "vec3.h"

#define MAX_UV_MAPS 2

namespace VertexAttrib {
	enum {
		IDX = 0,
		POS = 1 << 0,
		NML = 1 << 1,
		UV0 = 1 << 2,
		UV1 = 1 << 3,
	};
};

struct MeshData {
	
	uint32_t vtx_attribs  = VertexAttrib::POS;

	size_t idx_capacity   = 0;
	uint32_t *indices     = nullptr;

	size_t vtx_capacity   = 0;
	Vec3 *positions       = nullptr;
	Vec3 *normals         = nullptr;
	Vec2 *uv[MAX_UV_MAPS] = {nullptr};

	void clear();
	void reserve_indices (size_t count, bool shrink = false);
	void reserve_vertices(size_t count, bool shrink = false);
};

struct Mesh {
	uint32_t index_offset;
	uint32_t index_count;
	uint32_t vertex_offset;
	uint32_t vertex_count;
};


