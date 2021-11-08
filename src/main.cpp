#include <stdlib.h>
#include <stdio.h>
#include <GLFW/glfw3.h>

#include "Version.h"
#include "Viewer3D.h"

bool InitWindowSystem()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	return glfwInit();
}

void CloseWindowSystem()
{
	glfwTerminate();
}



int main(int argc, char **argv)
{

	if (!InitWindowSystem())
	{
		printf("Could not init windowing system.\n");
		return EXIT_FAILURE;
	}

	/* Main window */
	Viewer3D Viewer;
	
	if ( !Viewer.Init(1024, 768, "3D Viewer") )
	{
		printf("Failed to init viewer.\n");
		CloseWindowSystem();
		return EXIT_FAILURE;
	}

	while (!glfwWindowShouldClose(Viewer.Window)) {
		Viewer.ProcessKeys();
		glfwSwapBuffers(Viewer.Window);
		glfwPollEvents();
	}
	glfwTerminate();
	return EXIT_SUCCESS;
}
