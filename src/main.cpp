#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "meshoptimizer/src/meshoptimizer.h"

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include "version.h"
#include "aabb.h"
#include "transform.h"
#include "viewer.h"
#include "mesh_io.h"
#include "mesh_grid.h"
#include "mesh_utils.h"
#include "chrono.h"
#include "shaders.h"
#include "myosotis.h"

void syntax(char *argv[])
{
	printf("Syntax : %s mesh_file_name [max_level]\n", argv[0]);
}

void meshopt_statistics(const char *name, const MBuf& data, 
			const Mesh& mesh)
{
	uint32_t *idx = data.indices + mesh.index_offset;
	float *pos = (float*)(data.positions + mesh.vertex_offset);
	uint32_t nidx = mesh.index_count;
	uint32_t nvtx = mesh.vertex_count;
	
	const size_t kCacheSize = 16;
	meshopt_VertexCacheStatistics vcs = meshopt_analyzeVertexCache(
				idx, nidx, nvtx, kCacheSize, 0, 0);
	meshopt_VertexFetchStatistics vfs = meshopt_analyzeVertexFetch(
				idx, nidx, nvtx, sizeof(Vec3));
	meshopt_OverdrawStatistics os = meshopt_analyzeOverdraw(
				idx, nidx, pos, nvtx, sizeof(Vec3));
	meshopt_VertexCacheStatistics vcs_nv = meshopt_analyzeVertexCache(
			idx, nidx, nvtx, 32, 32, 32);
	meshopt_VertexCacheStatistics vcs_amd = meshopt_analyzeVertexCache(
			idx, nidx, nvtx, 14, 64, 128);
	meshopt_VertexCacheStatistics vcs_intel = meshopt_analyzeVertexCache(
			idx, nidx, nvtx, 128, 0, 0);

	printf("%-9s: ACMR %.2f ATVR %.2f (NV %.2f AMD %.2f Intel %.2f) "
	       "Overfetch %.2f Overdraw %.2f\n", name, vcs.acmr, vcs.atvr, 
	       vcs_nv.atvr, vcs_amd.atvr, vcs_intel.atvr, vfs.overfetch, 
	       os.overdraw);
}

void meshopt_optimize(MBuf& data, const Mesh& mesh)
{
	uint32_t *idx = data.indices + mesh.index_offset;
	float *pos = (float*)(data.positions + mesh.vertex_offset);
	float *nml = (float*)(data.normals + mesh.vertex_offset);
	float *uv0 = (float*)(data.uv[0] + mesh.vertex_offset);
	uint32_t nidx = mesh.index_count;
	uint32_t nvtx = mesh.vertex_count;
		
	meshopt_optimizeVertexCache(idx, idx, nidx, nvtx);
		
	const float kThreshold = 1.01f;
	meshopt_optimizeOverdraw(idx, idx, nidx, pos, nvtx, sizeof(Vec3), 
				 kThreshold);

	TArray<unsigned int> remap(nvtx);
	meshopt_optimizeVertexFetchRemap(&remap[0], idx, nidx, nvtx);
	meshopt_remapIndexBuffer(idx, idx, nidx, &remap[0]);
	meshopt_remapVertexBuffer(pos, pos, nvtx, sizeof(Vec3), &remap[0]);
	if (data.vtx_attr & VtxAttr::NML)
	{
		meshopt_remapVertexBuffer(nml, nml, nvtx, sizeof(Vec3), 
					  &remap[0]);
	}
	if (data.vtx_attr & VtxAttr::UV0)
	{
		meshopt_remapVertexBuffer(uv0, uv0, nvtx, sizeof(Vec2),
					  &remap[0]);
	}
}

int main(int argc, char **argv)
{
	if (argc <= 1)
	{
		syntax(argv);
		return(EXIT_FAILURE);
	}
	
	/* Load and process mesh */
	timer_start();
	MBuf data;
	Mesh mesh;

	{
		size_t len = strlen(argv[1]);
		const char* ext = argv[1] + (len-3);
		if (strncmp(ext, "obj", 3) == 0)
		{
			if (load_obj(argv[1], data, mesh))
			{
				printf("Error reading Wavefront file.\n");
				return (EXIT_FAILURE);
			}
		}
		else if (strncmp(ext, "ply", 3) == 0)
		{
			if (load_ply(argv[1], data, mesh))
			{
				printf("Error reading PLY file.\n");
				return (EXIT_FAILURE);
			}
		}
		else 
		{
			printf("Unsupported file type extension: %s\n", ext);
			return (EXIT_FAILURE);
		}
	}
	
	printf("Triangles : %d Vertices : %d\n", mesh.index_count / 3,
			mesh.vertex_count);
	timer_stop("loading mesh");

	if (argc > 2 && *argv[2] == '1')
	{
		timer_start();
		meshopt_statistics("Raw", data, mesh);
		timer_start();
		meshopt_optimize(data, mesh);
		timer_stop("optimize mesh");
		meshopt_statistics("Optimized", data, mesh);
	}

	if (!(data.vtx_attr & VtxAttr::NML))
	{
		timer_start();
		printf("Computing normals.\n");
		compute_mesh_normals(mesh, data);
		timer_stop("compute_mesh_normals");
	}

	timer_start();
	Aabb bbox = compute_mesh_bounds(mesh, data);
	Vec3 model_center = (bbox.min + bbox.max) * 0.5f;
	Vec3 model_extent = (bbox.max - bbox.min);
	float model_size = max(model_extent);
	timer_stop("compute_mesh_bounds");

	//if (argc > 3) 
	//{
		timer_start();
		uint32_t levels = atoi(argv[3]);
		float step = model_size / (1 << levels);
		Vec3 base = bbox.min;
		MeshGrid mg(base, step, levels);
		mg.build_from_mesh(data, mesh);
		timer_stop("split_mesh_with_grid");
	//}
	
	/* Main window and context */
	Myosotis app;
	
	if (!app.init(1920, 1080))
	{
		return (EXIT_FAILURE);
	}

	/* Init camera position */	
	app.viewer.target = model_center;
	Vec3 start_pos = (model_center + 2.f * Vec3(0, 0, model_size));
	app.viewer.camera.set_position(start_pos);
	app.viewer.camera.set_near(0.001 * model_size);
	app.viewer.camera.set_far(100 * model_size);
	

	glEnable(GL_DEBUG_OUTPUT);

	/* Upload mesh */	

	/* Index buffer */
	GLuint idx;
	glGenBuffers(1, &idx);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		mesh.index_count * sizeof(uint32_t),
		data.indices + mesh.index_offset,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	
	/* Position buffer */
	GLuint pos;
	glGenBuffers(1, &pos);
	glBindBuffer(GL_ARRAY_BUFFER, pos);
	glBufferData(GL_ARRAY_BUFFER,
			mesh.vertex_count * sizeof(Vec3),
			data.positions + mesh.vertex_offset,
			GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Normal buffer */
	GLuint nml;
	glGenBuffers(1, &nml);
	glBindBuffer(GL_ARRAY_BUFFER, nml);
	glBufferData(GL_ARRAY_BUFFER,
			mesh.vertex_count * sizeof(Vec3),
			data.normals + mesh.vertex_offset,
			GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Setup VAOs */

	/* Default VAO */
	GLuint default_vao;
	glGenVertexArrays(1, &default_vao);
	glBindVertexArray(default_vao);
	glBindBuffer(GL_ARRAY_BUFFER, pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 
				(void *)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, nml);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT),
				(void *)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/* Vertex fetch VAO */
	GLuint fetch_vao;
	glGenVertexArrays(1, &fetch_vao);
	glBindVertexArray(fetch_vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	/* Vertex pull VAO */
	GLuint null_vao;
	glGenVertexArrays(1, &null_vao);
	glBindVertexArray(null_vao);
	glBindVertexArray(0);

	/* Setup programs */
	
	GLint mesh_prg = create_shader("./shaders/default.vert", 
					  "./shaders/default.frag");
	if (mesh_prg < 0) 
	{
		return EXIT_FAILURE;
	}
	
	GLint fetch_mesh_prg = create_shader("./shaders/fetch_mesh.vert", 
					  "./shaders/default.frag");
	if (fetch_mesh_prg < 0) 
	{
		return EXIT_FAILURE;
	}
	
	GLint nml_prg = create_shader("./shaders/face_normals.vert", 
					  "./shaders/face_normals.frag");
	if (nml_prg < 0) 
	{
		return EXIT_FAILURE;
	}
	
	/* Setup some rendering options */

	//glDisable(GL_CULL_FACE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	/* Main loop */
	while (!app.should_close()) {
		
		app.new_frame();

		
		glClearColor(app.cfg.clear_color.x, app.cfg.clear_color.y, 
			     app.cfg.clear_color.z, app.cfg.clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Update uniform data */
		Mat4 proj = app.viewer.camera.view_to_clip();	
		Mat4 vm = app.viewer.camera.world_to_view();
		Vec3 camera_pos = app.viewer.camera.get_position();
		
		/* Draw mesh */
		if (app.cfg.draw_mesh)
		{
			glUseProgram(mesh_prg);
			glBindVertexArray(default_vao);
			glUniformMatrix4fv(0, 1, 0, &(vm.cols[0][0])); 
			glUniformMatrix4fv(1, 1, 0, &(proj.cols[0][0]));
			glUniform3fv(2, 1, &camera_pos[0]);
			glUniform1i(3, app.cfg.smooth_shading);
			glDrawElements(GL_TRIANGLES, mesh.index_count,
					GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
		else
		{
			//glUseProgram(fetch_mesh_prg);
			//glBindVertexArray(fetch_vao);
			//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pos);
			//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, nml);
			//glUniformMatrix4fv(0, 1, 0, &(vm.cols[0][0])); 
			//glUniformMatrix4fv(1, 1, 0, &(proj.cols[0][0]));
			//glUniform3fv(2, 1, &camera_pos[0]);
			//glUniform1i(3, app.cfg.smooth_shading);
			//glDrawElements(GL_TRIANGLES, mesh.index_count,
			//		GL_UNSIGNED_INT, 0);
			//glBindVertexArray(0);
			
			//glUseProgram(mesh_prg);
			//glBindVertexArray(default_vao);
			//glUniformMatrix4fv(0, 1, 0, &(vm.cols[0][0])); 
			//glUniformMatrix4fv(1, 1, 0, &(proj.cols[0][0]));
			//glUniform3fv(2, 1, &camera_pos[0]);
			//glUniform1i(3, app.cfg.smooth_shading);
			//for (size_t i = 0; i < ; ++i)
			//{
			//	glDrawElements(GL_TRIANGLES, mesh.index_count,
			//		GL_UNSIGNED_INT, 0);
			//}
			//glBindVertexArray(0);

		}


		/* Draw normals */
		if (app.cfg.draw_normals)
		{
			glUseProgram(nml_prg);
			glBindVertexArray(null_vao);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, idx);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pos);
			glUniformMatrix4fv(0, 1, 0, &(vm.cols[0][0])); 
			glUniformMatrix4fv(1, 1, 0, &(proj.cols[0][0]));
			glUniform3fv(2, 1, &camera_pos[0]);
			glDrawArrays(GL_LINES, 0, 2 * mesh.index_count / 3);
			glBindVertexArray(0);
		}
		
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwSwapBuffers(app.window);
	
	}

	/* Cleaning */
	app.clean();
	
	return (EXIT_SUCCESS);
}
