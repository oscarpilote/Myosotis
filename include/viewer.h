#pragma once

#include <GLFW/glfw3.h>

#include "camera.h"

struct Viewer3D {
	
	GLFWwindow* window;
	int width;
	int height;
	bool resized;

	Camera camera;

	Vec3 target = Vec3::Zero;

	bool  is_mouse_pressed = false;
	int   button;
	int   mods;
	float last_click_x;
	float last_click_y;
	Vec3  last_camera_pos;
	Quat  last_camera_rot;
	Vec3  last_trackball_v;

	bool init(int width, int height, const char *title);
	void process_keys();
	void mouse_pressed(float px, float py, int button, int mods);
	void mouse_released(int button, int mods);
	void mouse_move(float px, float py);
	void mouse_scroll(float xoffset, float yoffset);
};
