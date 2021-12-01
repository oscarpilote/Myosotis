#pragma once

#include <stdint.h>

#include "vec3.h"
#include "array.h"
#include "hash_table.h"
#include "mesh.h"

struct Grid {
	Vec3 base;
	float step;
};

union CellCoord {
	struct {
		int16_t x;
		int16_t y;
		int16_t z;
		int16_t unused;
	};
	uint64_t key;
};

struct CellCoordHasher {
	static constexpr CellCoord empty_key = {0, 0, 0, 1};
	size_t hash(CellCoord coord) const {return coord.key;}
	bool is_empty(CellCoord coord) const {return coord.unused != 0;}
	bool is_equal(CellCoord c1, CellCoord c2) const {return c1.key == c2.key;}
};
typedef HashTable<CellCoord, uint32_t, CellCoordHasher> CellTable;

void split_mesh_with_grid(
		const Grid grid,
		const MeshData& in,
		const Mesh& mesh,
		MeshData& out,
		TArray<Mesh>& cells,
		CellTable& coord_to_cell_idx);

size_t    getBoxNumber(const struct MeshGridBook *b, uint64_t box_coord);
void*     getBoxVertices(const struct MeshGrid *mg, size_t box);
void*     getBoxVertexParents(const struct MeshGrid *mg, size_t box);
uint32_t* getBoxIndices(const struct MeshGrid *mg, size_t box);
size_t    getBoxVertexCount(const struct MeshGrid *mg, size_t box);
size_t    getMaxVertexCount(const struct MeshGrid *mg);
size_t    getTotalVertexCount(const struct MeshGrid *mg);
size_t    getBoxIndexCount(const struct MeshGrid *mg, size_t box);
size_t    getMaxIndexCount(const struct MeshGrid *mg);
size_t    getTotalIndexCount(const struct MeshGrid *mg);

/* 
 * TODO parameters meaning
 */ 
void buildMeshGrid(struct MeshGrid *mg, const void *vertices, size_t vertex_count, size_t vertex_stride, const uint32_t *indices, size_t index_count);

void buildMeshGridBook(struct MeshGridBook *b, const struct MeshGrid *m); 

void getBoxNeighbours(uint64_t *neighbours_coord, uint64_t box_coord);

void getBoxCorners(uint64_t *corners_coord, uint64_t box_coord);

size_t loadNeighbours(const struct MeshGrid *mg, const struct MeshGridBook *b, uint64_t box_coord, size_t *box, uint32_t *dest_indices, void *dest_vertices, size_t *dest_index_count, size_t *dest_vertex_count, bool parents);

size_t loadSubGrid(const struct MeshGrid *mg, const struct MeshGridBook *b, uint64_t bottom, size_t width, size_t *boxes, uint32_t *dest_indices, void *dest_vertices, size_t *dest_index_count, size_t *dest_vertex_count, bool parents, bool test_only);

