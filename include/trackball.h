#pragma once

#include <assert.h>

#include "vec3.h"
#include "quat.h"

struct Trackball {
	float width;
	float height;
	float radius;
	Vec3 saved_position;
	
	Trackball() = default;
	Trackball(float width, float height, float radius);
	Vec3 vect_at_click(float x, float y) const;
	void track_from(float x, float y);
	Quat get_rotation_to(float x, float y) const;
	void resize(float width, float height, float radius);
};

