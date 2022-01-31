#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

#include "meshoptimizer/src/meshoptimizer_mod.h"

#include "mesh_grid.h"
#include "vec3.h"
#include "hash_table.h"
#include "mesh.h"
#include "mesh_utils.h"

#include "chrono.h"

static inline
void point_to_cell_coord(CellCoord& coord, const Vec3& p, const Vec3& base, 
		float inv_step)
{
	Vec3 float_coord = (p - base) * inv_step;
	
	coord.x = floor(float_coord.x);
	coord.y = floor(float_coord.y);
	coord.z = floor(float_coord.z);
}

CellCoord parent_coord(const CellCoord coord)
{
	CellCoord pcoord;

	pcoord.lod = coord.lod + 1;
	pcoord.x = (coord.x - (coord.x < 0)) / 2;
	pcoord.y = (coord.y - (coord.y < 0)) / 2;
	pcoord.z = (coord.z - (coord.z < 0)) / 2;

	return pcoord;
}

CellCoord child_coord(const CellCoord coord, uint32_t i)
{
	assert(i < 8);
	CellCoord ccoord;
	
	ccoord.lod = coord.lod - 1;
	ccoord.x = 2 * coord.x + ((i >> 0) & 1);
	ccoord.y = 2 * coord.y + ((i >> 1) & 1);
	ccoord.z = 2 * coord.z + ((i >> 2) & 1);

	return ccoord;
}

Mesh* MeshGrid::get_cell(CellCoord ccoord)
{
	uint32_t *p = cell_table.get(ccoord);
	return p ? &cells[*p] : NULL;
}

unsigned MeshGrid::get_children(CellCoord pcoord, Mesh* children[8])
{
	unsigned child_count = 0;

	for (int i = 0; i < 8; ++i)
	{
		CellCoord ccoord = child_coord(pcoord, i);
		uint32_t *p = cell_table.get(ccoord);
		if (p)
		{
			children[child_count] = &cells[*p];
			child_count++;
		}
	}

	return child_count;
}

MeshGrid::MeshGrid(Vec3 base, float step, uint32_t levels): 
	base{base}, step{step},	levels{levels},
	cell_offsets(levels), cell_counts(levels), 
	cell_table(1 << (2 * levels + 3))
{
}

void MeshGrid::build_level(uint32_t level)
{

	/* Base level is built elsewhere */
	assert(level > 0);

	cell_offsets[level] = cells.size;
	cell_counts[level] = 0;

	/* Discover cell coords and update counts */
	for (uint32_t i = cell_offsets[level - 1]; i < cell_offsets[level]; ++i)
	{
		CellCoord ccoord = cell_coords[i];
		CellCoord pcoord = parent_coord(ccoord);
		uint32_t cell_idx;
		uint32_t *p = cell_table.get(pcoord);
		if (p)
		{
			continue;
		}

		/* Record parent cell */
		cell_idx = cell_table.size();
		cell_table.set_at(pcoord, cell_idx);
		cells.push_back(Mesh{0, 0, 0, 0});
		cell_coords.push_back(pcoord);
		cell_counts[level]++;

		/* Build parent cell */
		build_parent_cell(pcoord);
	}
}

void MeshGrid::build_from_mesh(const MBuf& src, const Mesh& mesh)
{
	data.vtx_attr = src.vtx_attr | VtxAttr::MAP;
	
	init_from_mesh(src, mesh);
	printf("Number of cells at level 0 : %d\n", cell_counts[0]);

	for (uint32_t level = 1; level < levels; level++)
	{
		build_level(level);
		printf("Number of cells at level %d : %d\n", level, 
				cell_counts[level]);
	}
}



void MeshGrid::init_from_mesh(const MBuf& src, const Mesh& mesh)
{
	cell_offsets[0] = 0;
	cell_counts[0] = 0;

	/* Loop over triangles and fill triangle to cell table. */
	
	size_t tri_count = mesh.index_count / 3;	
	TArray<uint32_t> tri_idx_to_cell_idx(tri_count);
	
	CellCoord cell_coord {{0, 0, 0, 0}};
	float inv_step = 1.f / step;
	for (size_t tri_idx = 0; tri_idx < tri_count; ++tri_idx)
	{
		const uint32_t *indices = src.indices + mesh.index_offset;
		const Vec3 *positions = src.positions + mesh.vertex_offset; 

		const Vec3 v1 = positions[indices[3 * tri_idx + 0]];
		const Vec3 v2 = positions[indices[3 * tri_idx + 1]];
		const Vec3 v3 = positions[indices[3 * tri_idx + 2]];

		const Vec3 bary = (v1 + v2 + v3) * (1.f / 3.f);
		
		point_to_cell_coord(cell_coord, bary, base, inv_step);
		
		uint32_t cell_idx;
		uint32_t *p = cell_table.get(cell_coord);
		if UNLIKELY(!p)
		{
			cell_idx = cell_table.size();
			cell_table.set_at(cell_coord, cell_idx);
			cells.push_back(Mesh{0, 0, 0, 0});
			cell_coords.push_back(cell_coord);
			cell_counts[0]++;
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
	data.reserve_indices(total_index_count);

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
		uint32_t *dst_idx = data.indices + dst_offset;
		uint32_t *src_idx = src.indices + src_offset;
		memcpy(dst_idx, src_idx, 3 * sizeof(uint32_t));
		
		cell.index_count += 3;
	}

	/* Clean-up */
	tri_idx_to_cell_idx.clear();

	/* Reserve vertices (total number is unknown at this point) */
	size_t vertex_count_guess = mesh.vertex_count + mesh.vertex_count / 4;
	data.reserve_vertices(vertex_count_guess);
	

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

		data.reserve_vertices(total_vertex_count + cell.index_count);

		assert(cell.vertex_count == 0);

		uint32_t *cell_indices = data.indices + cell.index_offset;

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
				copy_vertices(data, idx_d, src, idx_s, 1);
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

	next_index_offset = total_index_count;
	next_vertex_offset = total_vertex_count;
}

void MeshGrid::build_parent_cell(CellCoord pcoord)
{
	Mesh* target_mesh = get_cell(pcoord);
	assert(target_mesh);

	/* Get children */
	Mesh* children[8];
	unsigned child_count = get_children(pcoord, children);	
	assert(child_count);
	//printf("N childs : %d\n", child_count);

	/* Compute vtx and idx counts prior to simplification */
	uint32_t idx_count = 0;
	uint32_t vtx_count = 0;
	for (uint32_t i = 0; i < child_count; ++i)
	{
		idx_count += children[i]->index_count;
		vtx_count += children[i]->vertex_count;
	}
	//printf("idx_count %d vtx_count %d\n", idx_count, vtx_count);

	/* Prepare structures for simplification */

	MBuf tmp_data;
	tmp_data.vtx_attr = data.vtx_attr;
	//printf("Reserving %d and %d:\n", idx_count, vtx_count);
	tmp_data.reserve_indices(idx_count);
	tmp_data.reserve_vertices(vtx_count + 1);
	
	VertexTable tmp_table(vtx_count + 16, &tmp_data, tmp_data.vtx_attr);
	
	TArray<uint32_t> remap(vtx_count);
	
	Mesh tmp_mesh {0, 0, 0, 0};
	
	/* Join meshes */
	uint32_t *tmp_remap = remap.data;
	for (uint32_t i = 0; i < child_count; ++i)
	{
		join_mesh(tmp_mesh, tmp_data, *children[i], data, tmp_table, 
				tmp_remap);
		tmp_remap += children[i]->vertex_count;
	}

	/* Simplify group */
	//timer_start();
	//uint32_t tri_in = tmp_mesh.index_count / 3;
	TArray<uint32_t> remap2(tmp_mesh.vertex_count);
	tmp_mesh.index_count = meshopt_simplify_mod(
			tmp_data.indices, remap2.data, tmp_data.indices, 
			tmp_mesh.index_count, (const float*)tmp_data.positions, 
			tmp_mesh.vertex_count, 3 * sizeof(float), 
			tmp_mesh.index_count / 4, 1, NULL);

	//uint32_t tri_out = tmp_mesh.index_count / 3;
	//unsigned int mus = timer_stop();
	//printf("Simplify %d to %d at %.2f Mtri/sec\n", tri_in, tri_out, 
	//		(float)tri_in / mus);

	/* Update remap after simplification */
	for (uint32_t i = 0; i < vtx_count; i++)
	{
		remap[i] = remap2[remap[i]];
	}
	//printf("Updated remap\n");

	/* Write back group to mesh_grid */
	tmp_table.clear();
	tmp_table.set_mesh_data(&data);
	target_mesh->index_offset = next_index_offset;
	target_mesh->vertex_offset = next_vertex_offset;
	//printf("Reserving %d indices and %d vertices\n", next_index_offset + idx_count, next_vertex_offset + vtx_count);
	data.reserve_indices(next_index_offset + idx_count);
	data.reserve_vertices(next_vertex_offset + vtx_count + 1);
	//printf("Join mesh\n");
	join_mesh(*target_mesh, data, tmp_mesh, tmp_data, tmp_table, remap2.data); 
	//printf("Done\n");
	assert(idx_count >= target_mesh->index_count && vtx_count >= target_mesh->vertex_count);
	next_index_offset += target_mesh->index_count;
	next_vertex_offset += target_mesh->vertex_count;
	//printf("nio %d nvo %d\n", next_index_offset, next_vertex_offset);
	//printf("Write back\n");

	/* Update remap after write back */
	for (uint32_t i = 0; i < vtx_count; i++)
	{
		//printf("Remap[i] : %d\n", remap[i]);
		remap[i] = remap2[remap[i]];
	}
	//printf("Updated remap (twice)\n");

	/* Update child vertices parent map using remap */
	tmp_remap = remap.data;
	for (unsigned i = 0; i < child_count; i++)
	{
		uint32_t* p = data.remap + children[i]->vertex_offset;
		for (uint32_t j = 0; j < children[i]->vertex_count; j++)
		{
			p[j] = tmp_remap[j];
		}
		tmp_remap += children[i]->vertex_count;
	}
	//printf("Updated parents done\n");
}

static void batch_simplify(Mesh *mesh, size_t num, MBuf& data, uint32_t *remap,
		MBuf& tmp_data, VertexTable& tmp_table)
{
	/* Reset vertex table and make sure tmp_data has enough free space */
	size_t vtx_count = 0;
	size_t idx_count  = 0;
	for (size_t i = 0; i < num; ++i)
	{
		vtx_count += mesh[i].vertex_count;
		idx_count  += mesh[i].index_count;
	}
	tmp_data.vtx_attr = data.vtx_attr;
	tmp_data.reserve_indices(idx_count);
	tmp_data.reserve_vertices(vtx_count);
	tmp_table.clear();
	
	/* Join meshes */
	Mesh group {0, 0, 0, 0};
	uint32_t *remap_loc = remap;
	for (size_t i = 0; i < num; ++i)
	{
		join_mesh(group, tmp_data, mesh[i], data, tmp_table, remap_loc);
		remap_loc += mesh[i].vertex_count;
	}
	
	/* Simplify group */
	TArray<uint32_t> s_remap(group.vertex_count);
	//simplify_mesh(group, tmp_data, &s_remap[0]);

	/* Update remap after simplification */
	for (size_t i = 0; i < vtx_count; ++i)
	{
		assert(remap[i] < group.vertex_count);
		remap[i] = s_remap[remap[i]];
	}
}
