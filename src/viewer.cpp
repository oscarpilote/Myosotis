#include <string.h>
#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "viewer.h"
#include "camera.h"
#include "trackball.h"
#include "transform.h"

#define DOUBLE_CLICK_TIME 0.3   /* in seconds           */
#define ZOOM_SENSITIVITY 0.3f   /* for mouse wheel zoom */

static void
resize_window_callback(GLFWwindow* window, int width, int height)
{
	Viewer3D* viewer = (Viewer3D *)glfwGetWindowUserPointer(window);
	
	viewer->width = width;
	viewer->height = height;
	viewer->resized = true;
	viewer->camera.set_aspect((float)width / height);

	glViewport(0, 0, width, height);
}

static void
mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Viewer3D* viewer = (Viewer3D *)glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		viewer->mouse_pressed(xpos, ypos, button, mods);
	} 
	else if (action == GLFW_RELEASE)
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		viewer->mouse_released(button, mods);
	}
}

static void
cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	Viewer3D* viewer = (Viewer3D *)glfwGetWindowUserPointer(window);
	viewer->mouse_move(xpos, ypos);
}

static void
scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Viewer3D* viewer = (Viewer3D *)glfwGetWindowUserPointer(window);
	viewer->mouse_scroll(xoffset, yoffset);
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
	
	this->width = width;
	this->height = height;
	camera.set_aspect((float)width / height);
	camera.set_fov(90.f);
	
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
	/* Further key controls go here */
}

void Viewer3D::mouse_pressed(float px, float py, int button, int mods)
{
	static double last_pressed = -1.0;
	double now = glfwGetTime();
	bool double_click = (now - last_pressed) < DOUBLE_CLICK_TIME;
	last_pressed = now;

	
	is_mouse_pressed = true;
	this->button = button;
	this->mods = mods;
	last_click_x = px;
	last_click_y = py;
	last_camera_pos  = camera.get_position();
	last_camera_rot  = camera.get_rotation();
	last_trackball_v = screen_trackball(px, py, width, height, width / 2.f);
		
	if (!double_click) return;

	float pixels[1];
	float tx = px;
	float ty = height - py;
	glReadPixels(tx, ty, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, pixels);
	/* Don't track clicks outisde of model */
	if (approx_equal(pixels[0], 1.f)) 
	{
		return;
	}
	
	target = camera.world_coord_at(px / width, py / height, pixels[0]);
}

void Viewer3D::mouse_released(int button, int mods)
{
	(void)button;
	(void) mods;
	is_mouse_pressed = false;
}

void Viewer3D::mouse_move(float px, float py)
{
	if (!is_mouse_pressed) return;
	
	if (mods & GLFW_MOD_SHIFT)
	{
		/* Translation in x and y */	
		float dist = norm(target - camera.get_position());
		float mult = dist / width;
		Vec3 trans;
		trans.x = (last_click_x - px) * mult;
		trans.y = (py - last_click_y) * mult;
		trans.z = 0.f;
		camera.set_position(last_camera_pos);
		camera.translate(trans, Camera::View);
	}
	else if (mods & GLFW_MOD_CONTROL)
	{
		/* Zoom (translation in target direction) */
		float mu = ZOOM_SENSITIVITY * (px - last_click_x) / 100;
		Vec3 new_pos = target + exp(mu) * (last_camera_pos - target);
		camera.set_position(new_pos);
	}
	else /* no modifier */
	{
		/* Orbit around target */
		Vec3 trackball_v = screen_trackball(px, py, width, height, width / 2.f);
		Quat rot = great_circle_rotation(last_trackball_v, trackball_v);
		/* rot quat is in view frame, back to world frame */
		rot.xyz = rotate(rot.xyz, last_camera_rot);
		camera.set_position(last_camera_pos);
		camera.set_rotation(last_camera_rot);
		camera.orbit(-rot, target);
	}
}

void Viewer3D::mouse_scroll(float xoffset, float yoffset)
{

	Vec3 old_pos = camera.get_position();
	Vec3 new_pos = target + exp(-ZOOM_SENSITIVITY * yoffset) * (old_pos - target);
	camera.set_position(new_pos);
}
