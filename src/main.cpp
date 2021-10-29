#include <stdio.h>
#include <GLFW/glfw3.h>

#include "view.h"


struct ViewerWindow {
	GLFWwindow* Window;
	View3D View;
	ViewController Trackpad;

	bool Init(int width, int height, const char* title);
}; 

bool ViewerWindow::Init(int width, int height, const char *title)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	Window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!Window) 
	{
		glfwTerminate();
		return (false);
	}
	
	glfwSetWindowUserPointer(Window, this);
	glfwMakeContextCurrent(Window);

	View.SetPosition(0.0, 0.0, 0.0);
	View.SetRotation(0.0, 1.0, 0.0, 0.0);

	Trackpad.SetTargetView(&View);

	return (true);
}

void resize_window_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	ViewerWindow* Main = static_cast<ViewerWindow *>(glfwGetWindowUserPointer(window));
	
	if (action == GLFW_PRESS)
	{
		printf("Pressed !\n");
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		Main->Trackpad.SetLastClick(xpos, ypos);

		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			switch (mods) {
				case (GLFW_MOD_SHIFT):
					Main->Trackpad.SetMode(PAN);
					break;
				case (GLFW_MOD_CONTROL):
					Main->Trackpad.SetMode(ZOOM);
					break;
				default:
					Main->Trackpad.SetMode(ORBIT);
			}
			Main->Trackpad.bFollowMouse = true;
		}

	} 
	else if (action == GLFW_RELEASE)
	{
		Main->Trackpad.bFollowMouse = false;	
	}
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	ViewerWindow* Main = static_cast<ViewerWindow *>(glfwGetWindowUserPointer(window));
	if (Main->Trackpad.bFollowMouse)
	{
		Main->Trackpad.MoveTo(xpos, ypos);
	}
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ViewerWindow* Main = static_cast<ViewerWindow *>(glfwGetWindowUserPointer(window));
	Main->Trackpad.ScrollBy(xoffset, yoffset);
}

void process_keys(GLFWwindow *window)
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

int main(int argc, char **argv)
{

	ViewerWindow Main;

	if ( !Main.Init(1024, 768, "ViewerApp"))
	{
		printf("Failed to create GLFW window\n");
		return -1;
	}

	glfwSetFramebufferSizeCallback(Main.Window, resize_window_callback);
	glfwSetCursorPosCallback(Main.Window, cursor_position_callback);
	glfwSetMouseButtonCallback(Main.Window, mouse_button_callback);
	glfwSetScrollCallback(Main.Window, scroll_callback);

	
	while (!glfwWindowShouldClose(Main.Window)) {
		process_keys(Main.Window);
		glfwSwapBuffers(Main.Window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}
