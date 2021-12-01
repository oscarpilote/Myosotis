#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include "version.h"
#include "transform.h"
#include "viewer.h"
#include "mesh_io.h"
#include "mesh_grid.h"
#include "mesh_utils.h"
#include "chrono.h"

bool init_window_system()
{
	
	bool ret = glfwInit();


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	return ret;
}

void close_window_system()
{
	glfwTerminate();
}

static void MessageCallback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar* message,
		const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR) {
		printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x,\
			message = %s\n", type == GL_DEBUG_TYPE_ERROR ? 
			"** GL ERROR **" : "", type, severity, message);
	}
}

void syntax(char *argv[])
{
	printf("Syntax : %s mesh_file_name [max_level]\n", argv[0]);
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
	MeshData data;
	Mesh mesh;

	{
		size_t len = strlen(argv[1]);
		if (strncmp(argv[1] + (len-3), "obj", 3) == 0)
		{
			fastObjMesh *obj = fast_obj_read(argv[1]);
			if (obj == nullptr)
			{
				printf("Error reading Wavefront file.\n");
				return (EXIT_FAILURE);
			}
			obj_to_mesh(*obj, data, mesh);
			fast_obj_destroy(obj);
		}
		else if (strncmp(argv[1] + (len-3), "ply", 3) == 0)
		{
			if (ply_to_mesh(argv[1], data, mesh))
			{
				printf("Error reading PLY file.\n");
				return (EXIT_FAILURE);
			}
		}
		else 
		{
			printf("Unsupported file type: %s\n", argv[1]);
			return (EXIT_FAILURE);
		}
	}
	
	printf("Triangles : %d Vertices : %d\n", mesh.index_count / 3,
			mesh.vertex_count);
	timer_stop("loading mesh");

	if (!(data.vtx_attribs & VertexAttrib::NML))
	{
		timer_start();
		printf("Computing normals.\n");
		compute_mesh_normals(mesh, data);
		timer_stop("compute_mesh_normals");
	}

	timer_start();
	Bbox bbox = compute_mesh_bounds(mesh, data);
	Vec3 model_center = (bbox.min + bbox.max) * 0.5f;
	Vec3 model_extent = (bbox.max - bbox.min);
	float model_size = std::max(std::max(model_extent.x, model_extent.y), model_extent.z);
	timer_stop("compute_mesh_bounds");

	if (argc > 2) 
	{
		timer_start();
		int level = atoi(argv[2]);
		Vec3 extent = bbox.max - bbox.min;
		float cube_size = model_size;  
		Grid grid = {bbox.min, cube_size / (1 << level)};
		MeshData data2;
		TArray<Mesh> cells;
		CellTable coord_to_cell_idx(1 << (2 * level + 3));
		split_mesh_with_grid(grid, data, mesh, data2, cells, coord_to_cell_idx);	
		printf("Cells : %zu\n", cells.size);
		timer_stop("split_mesh_with_grid");
	}
	
	/* Init Window system */	
	if (!init_window_system())
	{
		printf("Could not init windowing system.\n");
		return (EXIT_FAILURE);
	}
	
	/* Main window and context */
	Viewer3D viewer;
	
	if ( !viewer.init(1024, 768, "3D Viewer") )
	{
		printf("Failed to init viewer.\n");
		close_window_system();
		return (EXIT_FAILURE);
	}
	
	/* Init camera position */	
	viewer.target = model_center;
	viewer.camera.set_position(model_center + Vec3(0, 0, model_size));
	viewer.camera.set_near(0.001 * model_size);
	viewer.camera.set_far(100 * model_size);


	/* Upload mesh */	
	GLuint idx, pos, nml, tex, vao;
	
	glEnable(GL_DEBUG_OUTPUT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDebugMessageCallback(MessageCallback, 0);
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &idx);
	glGenBuffers(1, &pos);
	glGenBuffers(1, &nml);
	glGenBuffers(1, &tex);

	glBindVertexArray(vao);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		mesh.index_count * sizeof(uint32_t),
		data.indices + mesh.index_offset,
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 
			(void *)0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER,
			mesh.vertex_count * sizeof(Vec3),
			data.positions + mesh.vertex_offset,
			GL_STATIC_DRAW);
	
	if (data.vtx_attribs & VertexAttrib::NML)
	{
		glBindBuffer(GL_ARRAY_BUFFER, nml);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
				3 * sizeof(GL_FLOAT), (void *)0);
		glEnableVertexAttribArray(1);
		glBufferData(GL_ARRAY_BUFFER,
				mesh.vertex_count * sizeof(Vec3),
				data.normals + mesh.vertex_offset,
				GL_STATIC_DRAW);
	}

	if (data.vtx_attribs & VertexAttrib::NML)
	{
		glBindBuffer(GL_ARRAY_BUFFER, tex);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
				2 * sizeof(GL_FLOAT), (void *)0);
		glEnableVertexAttribArray(2);
		glBufferData(GL_ARRAY_BUFFER,
				mesh.vertex_count * sizeof(Vec2),
				data.uv[0] + mesh.vertex_offset,
				GL_STATIC_DRAW);
	}
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	/* Create shaders */
	int success;
	GLchar infoLog[512];

	GLint vert = glCreateShader(GL_VERTEX_SHADER);
	FILE *f = fopen("./shaders/default.vert", "rb");
	if (!f) {
		printf("No vertex shader!\n");
		return (EXIT_FAILURE);
	}
	fseek(f, 0, SEEK_END);
	int fsize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *source[] = {NULL};
	source[0] = static_cast<char *>(malloc((fsize + 1) ));
	fread(source[0], 1, fsize, f);
	fclose(f);
	source[0][fsize] = '\0';
	glShaderSource(vert, 1, (const char* const*) source, NULL);
	glCompileShader(vert);
	glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
	if (!success) 
	{
    		glGetShaderInfoLog(vert, 512, NULL, infoLog);
		printf("ERROR: compiling of %s shader %s failed !\n%s\n", 
				"GL_VERTEX_SHADER", "vertex", infoLog);
	}

	GLint frag = glCreateShader(GL_FRAGMENT_SHADER);
	FILE *g = fopen("./shaders/default.frag", "rb");
	if (!g) {
		printf("No fragment shader!\n");
		return (EXIT_FAILURE);
	}
	fseek(g, 0, SEEK_END);
	int gsize = ftell(g);
	fseek(g, 0, SEEK_SET);
	char *source2[] = {NULL};
	source2[0] = static_cast<char *>(malloc((gsize + 1)));
	fread(source2[0], 1, gsize, g);
	fclose(g);
	source2[0][gsize] = '\0';
	glShaderSource(frag, 1, (const char* const*) source2, NULL);
	glCompileShader(frag);
	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
	if (!success) 
	{
    		glGetShaderInfoLog(frag, 512, NULL, infoLog);
		printf("ERROR: compiling of %s shader %s failed !\n%s\n", 
				"GL_FRAGMENT_SHADER", "frag", infoLog);
		return (EXIT_FAILURE);
	}

	GLint prg = glCreateProgram();
	glAttachShader(prg, vert);
	glAttachShader(prg, frag);
	glLinkProgram(prg);
	glGetProgramiv(prg, GL_LINK_STATUS, &success);
	if (!success) 
	{
		printf("Failed to link shaders !\n");
		return (EXIT_FAILURE);
	}
	glDetachShader(prg, vert);
	glDetachShader(prg, frag);
	glDeleteShader(vert);
	glDeleteShader(frag);


	glfwSwapInterval(1);

	/* Main loop */
	while (!glfwWindowShouldClose(viewer.window)) {
		
		glfwPollEvents();

		glUseProgram(prg);
		glBindVertexArray(vao);
		
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Mat4 proj = viewer.camera.view_to_clip();	
		Mat4 vm = viewer.camera.world_to_view();
		Vec3 camera_pos = viewer.camera.get_position();

		glUniformMatrix4fv(0, 1, 0, &(vm.cols[0][0])); 
		glUniformMatrix4fv(1, 1, 0, &(proj.cols[0][0]));
		glUniform3fv(2, 1, &camera_pos[0]);
		glUniform1i(3, viewer.smooth_shading);
		glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT,
				0);
	
		glfwSwapBuffers(viewer.window);
	
	}

	/* Cleaning */
	close_window_system();
	
	return (EXIT_SUCCESS);
}
