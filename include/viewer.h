#pragma once

#include <GLFW/glfw3.h>

#include "camera.h"
#include "trackball.h"

struct Viewer3D {
	
	GLFWwindow* window;
	Camera camera;
	Trackball trackball;
	
	bool init(int width, int height, const char *title);
	void process_keys();
	void process_mouse();
};
