#include <stdio.h>
#include "fast_obj.h"
#include "meshoptimizer.h"

#include "mesh_io.h"
#include "array.h"
#include "hash_table.h"
#include "vec3.h"
#include "vec2.h"

Mesh obj_to_mesh(const char* filename)
{
	fastObjMesh* obj = fast_obj_read(filename);
	if (!obj)
	{
		printf("Error loading %s: file not found\n", filename);
		return Mesh();
	}
	
	size_t total_indices = 0;

	for (unsigned int i = 0; i < obj->face_count; ++i)
	{
		total_indices += 3 * (obj->face_vertices[i] - 2);
	}

	/*TArray<unsigned> indices(total_indices);
	for (size_t idx = 0; idx < total_indices; idx++)
	{
		indices[idx] = idx;
	}*/
	TArray<Vec3> positions(total_indices);
	TArray<Vec3> normals(total_indices);
	TArray<Vec2> texcoords(total_indices);

	size_t idx_offset = 0;
	size_t idx = 0;

	for (unsigned i = 0; i < obj->face_count; ++i)
	{
		for (unsigned j = 0; j < obj->face_vertices[i]; ++j)
		{
			if (j >= 3)
			{
				positions[idx + 0] = positions[idx - 3];
				positions[idx + 1] = positions[idx - 1];
				normals[idx + 0]   = normals[idx - 3];
				normals[idx + 1]   = normals[idx - 1];
				texcoords[idx + 0] = texcoords[idx - 3];
				texcoords[idx + 1] = texcoords[idx - 1];
				idx += 2;
			}
			
			fastObjIndex pnt = obj->indices[idx_offset + j];

			positions[idx] = Vec3(&obj->positions[pnt.p * 3]);
			normals[idx]   = Vec3(&obj->normals[pnt.n * 3]);
			texcoords[idx] = Vec2(&obj->texcoords[pnt.t * 2]); 

			idx++;
		}

		idx_offset += obj->face_vertices[i];
	}

	fast_obj_destroy(obj);

	

	meshopt_Stream streams[3] { 
			{&positions[0], 3 * sizeof(float), 3 * sizeof(float)},
			{&normals[0]  , 3 * sizeof(float), 3 * sizeof(float)},
			{&texcoords[0], 2 * sizeof(float), 2 * sizeof(float)}};

	TArray<unsigned int> remap(total_indices);
	
	size_t unique_vertex_count = meshopt_generateVertexRemapMulti(
					&remap[0], NULL, total_indices, 
					total_indices, streams, 3);

	TArray<unsigned> indices(total_indices);
	meshopt_remapIndexBuffer(&indices[0], NULL, total_indices, &remap[0]);


	TArray<Vec3> new_pos(unique_vertex_count);
	meshopt_remapVertexBuffer(&new_pos[0], &positions[0], 
			total_indices, sizeof(Vec3), &remap[0]);
	positions.clear();
	
	TArray<Vec3> new_nml(unique_vertex_count);
	meshopt_remapVertexBuffer(&new_nml[0], &normals[0], 
			total_indices, sizeof(Vec3), &remap[0]);
	normals.clear();
	
	TArray<Vec2> new_tex(unique_vertex_count);
	meshopt_remapVertexBuffer(&new_tex[0], &texcoords[0], 
			total_indices, sizeof(Vec2), &remap[0]);
	texcoords.clear();
	remap.clear();

	Mesh out;

	out.indices = indices;
	out.positions = new_pos;
	out.normals = new_nml;
	out.texcoords = new_tex;
	/*out.positions = positions;
	out.normals = normals;
	out.texcoords = texcoords;*/

	return out;
}
