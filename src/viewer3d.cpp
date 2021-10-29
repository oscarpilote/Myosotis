#include <string.h>
#include <stdio.h>
#include <GLFW/glfw3.h>

#include "viewer3d.h"
#include "camera.h"
#include "trackball.h"
#include "vec3.h"
#include "quat.h"

static void
resize_window_callback(GLFWwindow* window, int width, int height)
{
	Viewer3D* viewer = static_cast<Viewer3D *>(glfwGetWindowUserPointer(window));

	glViewport(0, 0, width, height);
	viewer->trackball.resize(width, height, height);
	viewer->camera.resize_sensor(width, height);
}

static void
mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Viewer3D* viewer = static_cast<Viewer3D *>(glfwGetWindowUserPointer(window));
	
	if (action == GLFW_PRESS)
	{
		printf("Pressed !\n");
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		viewer->trackball.track_from(xpos, ypos);

	} 
	else if (action == GLFW_RELEASE)
	{
	}
}

static void
cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Viewer3D* viewer = static_cast<Viewer3D *>(glfwGetWindowUserPointer(window));
	if (viewer->bFollowMouse)
	{
	}
}

static void
scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Viewer3D* viewer = static_cast<Viewer3D *>(glfwGetWindowUserPointer(window));
}



bool Viewer3D::init(int width, int height, const char *title)
{

	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) 
	{
		return (false);
	}
	glfwSetWindowUserPointer(window, this);
	glfwMakeContextCurrent(window);

	camera.init(width, height, 45.0f);
	trackball.
	
	glfwSetFramebufferSizeCallback(window, resize_window_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	return (true);
}

void Viewer3D::process_keys()
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, 1);
	}
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) 
	{

	}
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) 
	{
	}
}

