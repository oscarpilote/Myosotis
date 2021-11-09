#include "vec3.h"
#include "quat.h"
#include "geometry.h"
#include "trackball.h"

#include <stdio.h>

Trackball::Trackball(float width, float height, float radius) : 
	width{width}, height{height}, radius{radius}, 
	saved_position{0.f, 0.f, 1.f} {}


/* Using (inverse) stereographic projection from a sphere of given
 * radius and tangent to the screen plane at position (width/2, height/2).
 * Click coordinates are assumed to go increasing from left to right
 * and from top to bottom, so (0,0) is upper left. */
Vec3 Trackball::vect_at_click(float x, float y) const
{
	float X = (x - 0.5f * width) / radius;
	float Y = (0.5 * height - y) / radius;
	float a = 2.f / (1.f + X * X + Y * Y);

	Vec3 v {a * X, a * Y, -1.f + a};
	
	assert(approx_equal<float>(norm(v), 1.f));

	return v;
}

void Trackball::track_from(float x, float y)
{
	saved_position = vect_at_click(x, y);
}

Quat Trackball::get_rotation_to(float x, float y) const
{
	Vec3 new_position = vect_at_click(x, y);

	return great_circle_rotation(saved_position, new_position);
}

void Trackball::resize(float width, float height, float radius)
{
	this->width = width;
	this->height = height;
	this->radius = radius;
}
