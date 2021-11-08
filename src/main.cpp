#include <stdlib.h>
#include <stdio.h>
#include <GLFW/glfw3.h>

#include "version.h"
#include "viewer.h"

bool init_window_system()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	return glfwInit();
}

void close_window_system()
{
	glfwTerminate();
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

	while (!glfwWindowShouldClose(viewer.window)) {
		viewer.process_keys();
		glfwSwapBuffers(viewer.window);
		glfwPollEvents();
	}
	glfwTerminate();
	return EXIT_SUCCESS;
}
