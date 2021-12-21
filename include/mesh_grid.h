#pragma once

#include <stdint.h>

#include "vec3.h"
#include "array.h"
#include "hash_table.h"
#include "hash.h"
#include "mesh.h"

struct Grid {
	Vec3 base;
	float step;
};

union alignas(8) CellCoord {
	struct {
		int16_t lod;
		int16_t x;
		int16_t y;
		int16_t z;
	};
	uint64_t key;
};

inline bool operator==(CellCoord c1, CellCoord c2)
{
	return ( c1.key == c2.key );
}

struct CellCoordHasher {
	static constexpr CellCoord empty_key = {{-1, 0, 0, 0}};
	size_t hash(CellCoord coord) const;
	bool is_empty(CellCoord coord) const;
	bool is_equal(CellCoord c1, CellCoord c2) const;
};
typedef HashTable<CellCoord, uint32_t, CellCoordHasher> CellTable;

inline size_t CellCoordHasher::hash(CellCoord coord) const
{
	return murmur2_64(0, coord.key);
}

inline bool CellCoordHasher::is_empty(CellCoord coord) const
{
	return (coord == empty_key);
}

inline bool CellCoordHasher::is_equal(CellCoord c1, CellCoord c2) const
{
	return (c1 == c2);
}

void split_mesh_with_grid(
		const Grid grid,
		const MeshData& in,
		const Mesh& mesh,
		MeshData& out,
		TArray<Mesh>& cells,
		CellTable& cell_table);

