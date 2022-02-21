#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "viewer.h"

struct MyosotisCfg 
{
	const char *glsl_version = "#version 150";
	
	bool   adaptative_lod  = true;
	bool   colorize_lod    = false;
	bool   draw_normals    = false;
	bool   smooth_shading  = false;
	bool   frustum_cull    = true;
	bool   wireframe_mode  = false;
	bool   freeze_vp       = false;
	bool   vsync           = true;
	float  camera_fov      = 45.0f;
	int    level           = 0;  
	float  kappa           = 3;
	ImVec4 clear_color     = ImVec4(0.25f, 0.22f, 0.15f, 1.00f);
};

struct MyosotisStats
{
	int    drawn_cells = 0;
	int    drawn_tris = 0;	
};

struct Myosotis {

	/* Members */
	GLFWwindow* window;
	ImGuiIO*    io;
	Viewer3D    viewer;
	MyosotisCfg cfg;
	MyosotisStats stat;

	/* Methods */
	bool init(int width, int height);
	bool new_frame();
	bool should_close();
	bool clean();
};


