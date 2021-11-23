#include <stdlib.h>
#include <stdio.h>

//#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES = 1
//#endif
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include "version.h"
#include "transform.h"
#include "viewer.h"
#include "mesh_io.h"

bool init_window_system()
{
	
	bool ret = glfwInit();


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
	if (1) {//type == GL_DEBUG_TYPE_ERROR) {
		printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x,\
			message = %s\n", type == GL_DEBUG_TYPE_ERROR ? 
			"** GL ERROR **" : "", type, severity, message);
	}
}


int main(int argc, char **argv)
{
	
	
	if (!init_window_system())
	{
		printf("Could not init windowing system.\n");
		return EXIT_FAILURE;
	}
	
	/* Main window */
	Viewer3D viewer;
	
	if ( !viewer.init(1024, 768, "3D Viewer") )
	{
		printf("Failed to init viewer.\n");
		close_window_system();
		return EXIT_FAILURE;
	}
	
	Mesh m = obj_to_mesh(argv[1]);
	
	printf("Mesh size : %d %d %d %d\n", m.indices.size, m.positions.size, m.normals.size, m.texcoords.size);
	printf("Mesh positions : %f %f %f\n", m.positions[0][0], m.positions[6][1], m.positions[7][2]);
	GLuint idx, pos, nml, tex, vao;
	
	//glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_CULL_FACE);
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size * sizeof(unsigned), &m.indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void *)0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, m.positions.size * sizeof(Vec3), &m.positions[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, nml);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (void *)0);
	glEnableVertexAttribArray(1);
	glBufferData(GL_ARRAY_BUFFER, m.normals.size * sizeof(Vec3), &m.normals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, tex);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (void *)0);
	glEnableVertexAttribArray(2);
	glBufferData(GL_ARRAY_BUFFER, m.texcoords.size * sizeof(Vec2), &m.texcoords[0], GL_STATIC_DRAW);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	int success;
	GLchar infoLog[512];

	GLint vert = glCreateShader(GL_VERTEX_SHADER);
	FILE *f = fopen("./shaders/default.vert", "rb");
	if (!f) {
		printf("No vertex shader!\n");
		exit(0);
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
		printf("[gl_utils] ERROR: compiling of %s shader %s failed !\n%s\n", 
					"GL_VERTEX_SHADER", "vertex", infoLog);
	}

	GLint frag = glCreateShader(GL_FRAGMENT_SHADER);
	FILE *g = fopen("./shaders/default.frag", "rb");
	if (!g) {
		printf("No fragment shader!\n");
		exit(0);
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
		printf("[gl_utils] ERROR: compiling of %s shader %s failed !\n%s\n", 
					"GL_FRAGMENT_SHADER", "frag", infoLog);
	}

	GLint prg = glCreateProgram();
	glAttachShader(prg, vert);
	glAttachShader(prg, frag);
	glLinkProgram(prg);
	glGetProgramiv(prg, GL_LINK_STATUS, &success);
	if (!success) 
	{
		printf("Failed to link shaders !\n");
	}
	glDetachShader(prg, vert);
	glDetachShader(prg, frag);
	glDeleteShader(vert);
	glDeleteShader(frag);


	
	viewer.camera.set_position(Vec3(0, 0, 1));
	glfwSwapInterval(0);

	while (!glfwWindowShouldClose(viewer.window)) {
		
		glfwPollEvents();
		viewer.process_keys();


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
		glDrawElements(GL_TRIANGLES, m.indices.size, GL_UNSIGNED_INT, 0);
	
		glfwSwapBuffers(viewer.window);
	
	}

	close_window_system();
	
	return (EXIT_SUCCESS);
}
