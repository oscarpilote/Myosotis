#include "vec3.h"
#include "quat.h"
#include "mat4.h"

template <typename T>
class Camera 
{
private:
	/* Sensor size (unit is irrelevant but should match 
	 * with focal_length and lense_shift_[x/y] units below) */
	float sensor_width;
	float sensor_height;

	/* Lense focal length */
	float focal_length;

	/* Lense axis shift with respect to sensor center */
	float lense_shift_x = 0.f;
	float lense_shift_y = 0.f;

	/* Frustum depth */
	float near_plane = 1e-2f;
	float far_plane  = 1e+3f;

	/* Derived parameters (automatically updated by 
	 * setters of previous ones) */
	float aspect_x;		/*  2n/(r-l)   */
	float aspect_y;		/*  2n/(t-b)   */
	float shift_ratio_x;	/* (r+l)/(r-l) */
	float shift_ratio_y;	/* (t+b)/(t-b) */
	
	/* Projection type. True for orthographic camera. */
	bool is_orthographic = false;
	
	/* Space configuration	*/
	Vec3<T> pos = {0.f, 0.f, 0.f};
	Quat<T> rot = {1.f, {0.f, 0.f, 0.f}};

	/* Updating dervied parameters for consistency */ 
	void update_params();
	
public:
	/* Constructor */
	Camera(float sensor_width, float sensor_height, float lense_fov); 
	
	/* Accessors */
	float width() const;
	float height() const;
	float fov() const;
	float focal() const;
	float near() const;
	float far() const;
	bool  is_ortho() const;


	/* Modifiers   */
	Camera& resize_sensor(float width, float height, bool keep_const_fov);
	Camera& set_lense_fov(float deg /* horizontal fov */);
	Camera& set_lense_focal(float length);
	Camera& set_lense_shift(float x, float y);
	Camera& set_near(float near);
	Camera& set_far(float far);
	Camera& set_ortho(bool is_ortho);

	/* Space positioning */
	const Vec3<T>& get_position() const;
	const Quat<T>& get_rotation() const;
	Camera& set_position(const Vec3<T>& pos);
	Camera& set_rotation(const Quat<T>& rot);
	Camera& translate(const Vec3<T>& delta_world_pos);
	Camera& move(const Vec3<T>& delta_view_pos);
	Camera& rotate(const Quat<T>& delta_rot);
	Camera& orbit(const Quat<T>& delta_rot, const Vec3<T>& center);

	/* Matrices */
	Mat4<float> proj_matrix() const;
	Mat4<T> view_matrix() const;
	Mat4<T> proj_view_matrix() const;
};

