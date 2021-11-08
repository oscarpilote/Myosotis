#ifndef _VIEWER3D_H
#define _VIEWER3D_H

#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Trackball.h"

struct Viewer3D {
	
	GLFWwindow* Window;
	FCamera Camera;
	FTrackball Trackball;
	
	bool bFollowMouse;
	FVec3 TargetPosition;
	FQuat SavedRotation;
	
	bool Init(int Width, int Height, const char *Title);
	void ProcessKeys();
};

#endif /* _VIEWER3D_H */
