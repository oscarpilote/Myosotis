#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

#include "mesh_grid.h"
#include "vec3.h"
#include "hash_table.h"
#include "mesh.h"
#include "mesh_utils.h"

static CellCoord point_to_cell_coord(const Vec3& p, const Grid& grid)
{
	CellCoord coord;

	float invstep = 1.f / grid.step; 
	Vec3 float_coord = (p - grid.base) * invstep;
	
	coord.x = floor(float_coord.x);
	coord.y = floor(float_coord.y);
	coord.z = floor(float_coord.z);
	coord.unused = 0;
	
	return coord;  
}

void split_mesh_with_grid(
		const Grid grid,
		const MeshData& src,
		const Mesh& mesh,
		MeshData& dst,
		TArray<Mesh>& cells,
		CellTable& coord_to_cell_idx)
{
	dst.vtx_attribs = src.vtx_attribs;
	
	/* Loop over triangles and fill triangle to cell table. */
	
	size_t tri_count = mesh.index_count / 3;	
	TArray<uint32_t> tri_idx_to_cell_idx(tri_count);
	
	for (size_t tri_idx = 0; tri_idx < tri_count; ++tri_idx)
	{
		const uint32_t *indices = src.indices + mesh.index_offset;
		const Vec3 *positions = src.positions + mesh.vertex_offset; 

		const Vec3 v1 = positions[indices[3 * tri_idx + 0]];
		const Vec3 v2 = positions[indices[3 * tri_idx + 1]];
		const Vec3 v3 = positions[indices[3 * tri_idx + 2]];

		const Vec3 bary = (v1 + v2 + v3) * (1.f / 3.f);
		
		const CellCoord cell_coord = point_to_cell_coord(bary, grid);
		
		uint32_t cell_idx;
		uint32_t *p = coord_to_cell_idx.get(cell_coord);
		if UNLIKELY(!p)
		{
			cell_idx = coord_to_cell_idx.size();
			coord_to_cell_idx.set_at(cell_coord, cell_idx);
			cells.push_back(Mesh{0, 0, 0, 0});
		}
		else
		{
			cell_idx = *p;
		}
		
		cells[cell_idx].index_count += 3; 
		tri_idx_to_cell_idx[tri_idx] = cell_idx;
	}
	
	/* Compute index offsets and total index count */
	
	size_t total_index_count = 0; 
	
	for (size_t cell_idx = 0; cell_idx < cells.size; ++cell_idx)
	{
		cells[cell_idx].index_offset = total_index_count; 
		total_index_count += cells[cell_idx].index_count;
	}

	assert(total_index_count == mesh.index_count);
	
	/* Reserve indices */
	dst.reserve_indices(total_index_count);

	/* Reset cells index_count to zero momentarily */
	for (size_t cell_idx = 0; cell_idx < cells.size; ++cell_idx)
	{
		cells[cell_idx].index_count = 0;
	}

	/* Remap indices (and re-establish index counts) */
	for (size_t tri_idx = 0; tri_idx < tri_count; ++tri_idx)
	{
		Mesh& cell = cells[tri_idx_to_cell_idx[tri_idx]];
		
		size_t dst_offset = cell.index_offset + cell.index_count;
		size_t src_offset = mesh.index_offset + 3 * tri_idx;
		uint32_t *dst_idx = dst.indices + dst_offset;
		uint32_t *src_idx = src.indices + src_offset;
		memcpy(dst_idx, src_idx, 3 * sizeof(uint32_t));
		
		cell.index_count += 3;
	}

	/* Clean-up */
	tri_idx_to_cell_idx.clear();

	/* Reserve vertices (total number is unknown at this point) */
	size_t vertex_count_guess = mesh.vertex_count + mesh.vertex_count / 4;
	dst.reserve_vertices(vertex_count_guess);
	

	/**
	 * Compute max index_count in cells, this yields also an upper bound
	 * on max vertex_count per cell
	 */
	size_t max_index_count = 0;
	for (size_t cell_idx = 0; cell_idx < cells.size; ++cell_idx)
	{
		if (cells[cell_idx].index_count > max_index_count)
		{
			max_index_count = cells[cell_idx].index_count;
		}
	}
	HashTable<uint32_t, uint32_t> idx_remap(max_index_count); 	

	size_t total_vertex_count = 0;
	for (size_t cell_idx = 0; cell_idx < cells.size; ++cell_idx)
	{
		Mesh& cell = cells[cell_idx];
		cell.vertex_offset = total_vertex_count;

		dst.reserve_vertices(total_vertex_count + cell.index_count);

		assert(cell.vertex_count == 0);

		uint32_t *cell_indices = dst.indices + cell.index_offset;

		for (size_t i = 0; i < cell.index_count; ++i)
		{
			uint32_t old_idx = cell_indices[i];
			uint32_t new_idx;
			uint32_t *p = idx_remap.get(old_idx);
			if (!p)
			{
				new_idx = cell.vertex_count;
				idx_remap.set_at(old_idx, new_idx);
				size_t idx_s = mesh.vertex_offset + old_idx;
				size_t idx_d = total_vertex_count;
				copy_vertices(dst, idx_d, src, idx_s, 1);
				cell.vertex_count++;
				total_vertex_count++;
			}
			else
			{
				new_idx = *p;
			}
			cell_indices[i] = new_idx;
		}
		/* Clear idx_remap for use with next cell */
		idx_remap.clear();
	}
}

