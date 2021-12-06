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
	ImVec4 clear_color     = ImVec4(0.25f, 0.22f, 0.15f, 1.00f);
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


