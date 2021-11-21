#include <assert.h>
#include <math.h>

#include "camera.h"
#include "ndc.h"
#include "vec3.h"
#include "quat.h"
#include "mat4.h"
#include "geometry.h"
#include "transform.h"


Camera::Camera(float aspect_ratio, float fov, Fov axis)

{
	assert(aspect_ratio > 0.f && fov > 0.f);

	float focal_ratio = 1.f / tan(deg2rad(fov) / 2.f);

	if (axis == Horizontal)
	{
		frustum.aspect_x = focal_ratio;
		frustum.aspect_y = focal_ratio * aspect_ratio;

	}
	else
	{
		frustum.aspect_x = focal_ratio / aspect_ratio;
		frustum.aspect_y = focal_ratio;
	}
}

Camera& Camera::set_aspect(float aspect_ratio, const Fov cst_axis)
{

	assert(aspect_ratio > 0);

	if (cst_axis == Horizontal)
	{
		frustum.aspect_y = frustum.aspect_x * aspect_ratio;
	}
	else
	{
		frustum.aspect_x = frustum.aspect_y / aspect_ratio;
	}
	return (*this);
}

Camera& Camera::set_fov(float fov, Fov axis)
{
	float aspect_ratio = frustum.aspect_y / frustum.aspect_x;
	float focal_ratio = 1.f / tan(deg2rad(fov) / 2.f);

	if (axis == Horizontal)
	{
		frustum.aspect_x = focal_ratio;
		frustum.aspect_y = focal_ratio * aspect_ratio;

	}
	else
	{
		frustum.aspect_x = focal_ratio / aspect_ratio;
		frustum.aspect_y = focal_ratio;
	}
	return (*this);
}

Camera& Camera::set_lense_shift(float shift_x, float shift_y)
{
	frustum.shift_x = shift_x;
	frustum.shift_y = shift_y;
	return (*this);
}

Camera& Camera::set_orthographic(bool is_ortho)
{
	frustum.is_ortho = is_ortho;
	return (*this);
}

Vec3 Camera::get_position() const
{
	return (position);
}

Quat Camera::get_rotation() const
{
	return rotation;
}

Camera& Camera::set_position(const Vec3& position)
{
	this->position = position;
	return (*this);
}

Camera& Camera::set_rotation(const Quat& rotation)
{
	this->rotation = rotation;
	return (*this);
}

Camera& Camera::translate(const Vec3& t, Space coord)
{
	if (coord == View) 
	{
		position += ::rotate(t, rotation);
	}
	else /* World */
	{
		position += t;
	}
	return (*this);
}

Camera& Camera::rotate(const Quat& r)
{
	assert(!approx_equal(norm(r), 0.f));
	Quat rot = r * (1.f / norm(r));
	rotation = compose(rotation, rot);
	return (*this);
}

Camera& Camera::orbit(const Quat& r, const Vec3& pivot)
{
	assert(!approx_equal(norm(r), 0.f));
	Quat rot = r * (1.f / norm(r));
	rotation = compose(rotation, rot);
	position = ::orbit(position, rot, pivot);
	return (*this);
}

float Camera::get_near() const
{
	return frustum.near;
}

float Camera::get_far() const
{
	return frustum.far;
}

Camera& Camera::set_near(float near_plane)
{
	frustum.near = near_plane;
	return (*this);
}

Camera& Camera::set_far(float far_plane)
{
	frustum.far = far_plane;
	return (*this);
}

Mat4 Camera::view_to_clip() const
{
	return projection_matrix(frustum);
}

Mat4 Camera::clip_to_view() const
{
	return projection_matrix_inv(frustum);
}

Mat4 Camera::world_to_view() const
{
	return compose(-position, -rotation).as_matrix();
}

Mat4 Camera::view_to_world() const
{
	return compose(rotation, position).as_matrix();
}

Mat4 Camera::world_to_clip() const
{
	return compose(world_to_view(), view_to_clip());
}

Mat4 Camera::clip_to_world() const
{
	return compose(clip_to_view(), view_to_world());
}

Ray Camera::view_ray_at (float x, float y) const
{
	assert(0.f <= x && x <= 1.f && 0.f <= y && y <= 1.f);

	Vec3 ndc = nwd_to_ndc(x, y, 0.5f);
	Vec3 v = transform(clip_to_view(), ndc);
	
	return {.start = Vec3::Zero, .dir = v};
}

Ray Camera::world_ray_at(float x, float y) const
{
	assert(0.f <= x && x <= 1.f && 0.f <= y && y <= 1.f);

	Vec3 ndc = nwd_to_ndc(x, y, 0.5f);
	Vec3 v = transform(clip_to_world(), ndc);
	
	return {.start = position, .dir = v};
}

Vec3 Camera::view_coord_at (float x, float y, float depth) const
{
	assert(0.f <= x && x <= 1.f && 0.f <= y && y <= 1.f);
	assert(0.f <= depth && depth <= 1.f);

	Vec3 ndc = nwd_to_ndc(x, y, depth);
	
	return transform(clip_to_view(), ndc);

}

Vec3 Camera::world_coord_at(float x, float y, float depth) const
{
	assert(0.f <= x && x <= 1.f && 0.f <= y && y <= 1.f);
	assert(0.f <= depth && depth <= 1.f);

	Vec3 ndc = nwd_to_ndc(x, y, depth);
	
	return transform(clip_to_world(), ndc);
}

