#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "viewer.h"

struct MyosotisCfg 
{
	const char *glsl_version = "#version 150";
	
	bool   smooth_shading  = true;
	bool   vsync           = true;
	float  camera_fov      = 45.0f; 
	ImVec4 clear_color     = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

struct Myosotis {

	/* Members */
	GLFWwindow* window;
	ImGuiIO*    io;
	Viewer3D    viewer;
	MyosotisCfg cfg;

	/* Methods */
	bool init(int width, int height);
	bool new_frame();
	bool should_close();
	bool clean();
};


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
