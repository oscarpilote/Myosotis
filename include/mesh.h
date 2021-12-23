#pragma once

#include <stdint.h>

#include "vec2.h"
#include "vec3.h"

#define MAX_UV_MAPS 2

namespace VtxAttr {
	enum {
		POS = 0,
		NML = 1 << 0,
		UV0 = 1 << 1,
		UV1 = 1 << 2,
		PAR = 1 << 3,
		/* Some common combo */
		P   = POS,
		PN  = POS | NML,
		PNT = POS | NML | UV0,
		PT  = POS | UV0,
	};
};

/**
 * Mesh Buffer (host side)
 */
struct MBuf {
	
	uint32_t vtx_attr     = 0;

	size_t idx_capacity   = 0;
	uint32_t *indices     = nullptr;

	size_t vtx_capacity   = 0;
	Vec3 *positions       = nullptr;
	Vec3 *normals         = nullptr;
	Vec2 *uv[MAX_UV_MAPS] = {nullptr};
	uint32_t *parents     = nullptr;

	void clear();
	void reserve_indices (size_t num, bool shrink = false);
	void reserve_vertices(size_t num, bool shrink = false);
};

/**
 * Mesh (TODO : material) 
 */
struct Mesh {
	uint32_t index_offset;
	uint32_t index_count;
	uint32_t vertex_offset;
	uint32_t vertex_count;
};


