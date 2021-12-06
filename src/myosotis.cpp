#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GL/gl.h>
#include <GL/glext.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "myosotis.h"
#include "viewer.h"

static void
resize_window_callback(GLFWwindow* window, int width, int height);

static void
mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

static void
cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

static void
scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

static void
GL_debug_callback(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar* message,
		const void* user_param)
{
	(void) source;
	(void) length;
	(void) user_param;
	(void) id;
	if (type == GL_DEBUG_TYPE_ERROR) 
	{
		printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x,\
			message = %s\n", type == GL_DEBUG_TYPE_ERROR ? 
			"** GL ERROR **" : "", type, severity, message);
	}
}

bool Myosotis::init(int width, int height)
{
	/* Set-up GLFW */
	if (!glfwInit())
	{
		return (false);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
 	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
 
	//window = glfwCreateWindow(mode->width, mode->height, "Myosotis", 
	//glfwGetPrimaryMonitor(), NULL);
	window = glfwCreateWindow(width, height, "Myosotis", NULL, NULL);
	
	if (!window) 
	{
		return (false);
	}
	glfwSetWindowUserPointer(window, this);
	glfwMakeContextCurrent(window);

	glDebugMessageCallback(GL_debug_callback, nullptr);

	glfwSetKeyCallback(window, key_callback);	
	glfwSetFramebufferSizeCallback(window, resize_window_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSwapInterval(1);

	/* Set-up ImGUI */
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &ImGui::GetIO();
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(cfg.glsl_version);

	/* Set-up Viewer3D */
	viewer.init(width, height);

	return (true);
}

bool Myosotis::should_close()
{
	return  glfwWindowShouldClose(window);
}



bool Myosotis::new_frame()
{
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	
	ImGui::NewFrame();
	ImGui::Begin("Controls");
	ImGui::Checkbox("Smooth shading", &cfg.smooth_shading);
	if (ImGui::Checkbox("Use Vsync", &cfg.vsync))
	{
		glfwSwapInterval(cfg.vsync);	
	}
	if (ImGui::DragFloat("FOV", &cfg.camera_fov, 1, 1, 120, "%.0f"))
	{
		viewer.camera.set_fov(cfg.camera_fov);
	}
	ImGui::DragFloat("Trackball sensitivity", &viewer.sensitivity, 0.1,
		0.1, 2.0, "%.1f");
	ImGui::ColorEdit3("Background color", (float*)&cfg.clear_color);
	ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 
			1000.0f / io->Framerate, io->Framerate);
	ImGui::End();
	ImGui::Render();

	return (true);
}	

bool Myosotis::clean()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	return (true);
}

static void
resize_window_callback(GLFWwindow* window, int width, int height)
{
	Myosotis* app = (Myosotis*)glfwGetWindowUserPointer(window);
	
	app->viewer.width = width;
	app->viewer.height = height;
	app->viewer.camera.set_aspect((float)width / height);

	glViewport(0, 0, width, height);
}

static void
mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Myosotis* app = (Myosotis*)glfwGetWindowUserPointer(window);
	
	if (app->io->WantCaptureMouse)
	{
		return;
	}

	if (action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		app->viewer.mouse_pressed(xpos, ypos, button, mods);
	} 
	else if (action == GLFW_RELEASE)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		app->viewer.mouse_released(button, mods);
	}
}

static void
cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Myosotis* app = (Myosotis*)glfwGetWindowUserPointer(window);
	
	if (app->io->WantCaptureMouse)
	{
		return;
	}
	
	app->viewer.mouse_move(xpos, ypos);
}

static void
scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Myosotis* app = (Myosotis*)glfwGetWindowUserPointer(window);
	
	if (app->io->WantCaptureMouse)
	{
		return;
	}
	
	app->viewer.mouse_scroll(xoffset, yoffset);
}

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	(void) scancode;
	(void) mods;
	Myosotis* app = (Myosotis*)glfwGetWindowUserPointer(window);
	
	if (app->io->WantCaptureKeyboard)
	{
		return;
	}
	
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(app->window, 1);
		return;
	}
	
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		app->cfg.smooth_shading ^= true;
		return;
	}
	
	if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		app->viewer.nav_mode = NavMode::Orbit;
		return;
	}
	
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		app->viewer.nav_mode = NavMode::Free;
		return;
	}

	app->viewer.key_pressed(key, action);
}


