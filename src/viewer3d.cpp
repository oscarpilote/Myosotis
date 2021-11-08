#include <string.h>
#include <stdio.h>
#include <GLFW/glfw3.h>

#include "Viewer3D.h"
#include "Camera.h"
#include "Trackball.h"
#include "Vec3.h"
#include "Quat.h"

static void
resize_window_callback(GLFWwindow* window, int width, int height)
{
	Viewer3D* viewer = static_cast<Viewer3D *>(glfwGetWindowUserPointer(window));

	glViewport(0, 0, width, height);
	viewer->Trackball.Resize(width, height, height);
	viewer->Camera.ResizeSensor(width, height);
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
		viewer->Trackball.TrackFrom(xpos, ypos);

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



bool Viewer3D::Init(int width, int height, const char *title)
{

	Window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!Window) 
	{
		return (false);
	}
	glfwSetWindowUserPointer(Window, this);
	glfwMakeContextCurrent(Window);

	Camera.Init(width, height, 45.0f);
	Trackball.Resize(width, height, height);
	
	glfwSetFramebufferSizeCallback(Window, resize_window_callback);
	glfwSetCursorPosCallback(Window, cursor_position_callback);
	glfwSetMouseButtonCallback(Window, mouse_button_callback);
	glfwSetScrollCallback(Window, scroll_callback);

	return (true);
}

void Viewer3D::ProcessKeys()
{
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Window, 1);
	}
	if (glfwGetKey(Window, GLFW_KEY_KP_ADD) == GLFW_PRESS) 
	{

	}
	if (glfwGetKey(Window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) 
	{
	}
}

