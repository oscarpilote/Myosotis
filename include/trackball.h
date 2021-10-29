#ifndef _TRACKBALL_H
#define _TRACKBALL_H

#include <assert.h>

#include "math_utils.h"
#include "vec3.h"
#include "quat.h"

struct Trackball {
	float width;
	float height;
	float radius;
	Vec3<float> saved_point;
	Trackball() {};
	Trackball(float width, float height, float radius);
	Vec3<float> vect_at_click(float x, float y) const;
	void track_from(float x, float y);
	Quat<float> get_rotation_to(float x, float y) const;
	void resize(float width, float height, float radius);
};


#endif /* _TRACKBALL_H */
