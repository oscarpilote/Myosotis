#ifndef _VIEWER3D_H
#define _VIEWER3D_H

#include <GLFW/glfw3.h>

#include "camera.h"
#include "trackball.h"

struct Viewer3D {
	
	GLFWwindow* window;
	Camera<float> camera;
	Trackball trackball;
	
	bool bFollowMouse;
	Vec3<float> targetPosition;
	Quat<float> savedRotation;
	
	Viewer3D() {};
	bool init(int width, int height, const char *title);
	void process_keys();
};

#endif /* _VIEWER3D_H */
