#include <assert.h>

#include "frustum.h"
#include "gfx_api_defs.h" /* NDC_REVERSED_Y NDC_REVERSED_Z NDC_Z_ZERO_ONE */
#include "mat4.h"

Mat4 perspective_matrix(const CameraFrustum& f)
{
	assert(f.near != f.far);
	
	const float aspect_x = f.aspect_x;
	const float aspect_y = f.aspect_y;
	const float shift_x = f.shift_x;
	const float shift_y = f.shift_y;
	const float near = f.near;
	const float far = f.far;

	Mat4 M;

	/* col 0 */
	M(0,0) = aspect_x;
	M(1,0) = 0.f;
	M(2,0) = 0.f;
	M(3,0) = 0.f;

	/* col 1 */
	M(0,1) = 0.f;
	if constexpr( reversed_y) M(1,1) = -aspect_y;
	if constexpr(!reversed_y) M(1,1) = +aspect_y;
	M(2,1) = 0.f;
	M(3,1) = 0.f;

	/* col 2*/
	M(0,2) = shift_x;
	if constexpr( reversed_y) M(1,2) = -shift_y;
	if constexpr(!reversed_y) M(1,2) = +shift_y;
	if constexpr( z_zero_one &&  reversed_z) 
		M(2,2) = near / (far - near); 
	if constexpr( z_zero_one && !reversed_z) 
		M(2,2) = -1.f / (1.f - near / far);
	if constexpr(!z_zero_one &&  reversed_z) 
		M(2,2) = (1.f + near / far) / (1.f - near / far);
	if constexpr(!z_zero_one && !reversed_z) 
		M(2,2) = - (1.f + near / far) / (1.f - near / far);
	M(3,2) = -1.f;

	/* col 3 */
	M(0,3) = 0.f;
	M(1,3) = 0.f;
	if constexpr( z_zero_one &&  reversed_z) 
		M(2,3) = near / (1.f - near / far);
	if constexpr( z_zero_one && !reversed_z) 
		M(2,3) = -near / (1.f - near / far);
	if constexpr(!z_zero_one &&  reversed_z) 
		M(2,3) = 2.f * near / (1.f - near / far);
	if constexpr(!z_zero_one && !reversed_z) 
		M(2,3) = -2.f * near / (1.f - near / far);
	M(3,3) = 0.f;

	return (M);
}

Mat4 perspective_matrix_inv(const CameraFrustum& f)
{
	
	assert(f.near != 0 && f.far != 0);

	const float inv_aspect_x = 1.f / f.aspect_x;
	const float inv_aspect_y = 1.f / f.aspect_y;
	const float shift_x = f.shift_x;
	const float shift_y = f.shift_y;
	const float inv_near = 1.f / f.near;
	const float inv_far = 1.f / f.far;

	Mat4 M;

	/* col 0 */
	M(0,0) = inv_aspect_x;
	M(1,0) = 0.f;
	M(2,0) = 0.f;
	M(3,0) = 0.f;

	/* col 1 */
	M(0,1) = 0.f;
	if constexpr( reversed_y) M(1,1) = -inv_aspect_y;
	if constexpr(!reversed_y) M(1,1) = inv_aspect_y;
	M(2,1) = 0.f;
	M(3,1) = 0.f;

	/* col 2*/
	M(0,2) = 0.f;
	M(1,2) = 0.f;
	M(2,2) = 0.f;
	if constexpr( z_zero_one &&  reversed_z) 
		M(3,2) = inv_near - inv_far;
	if constexpr( z_zero_one && !reversed_z) 
		M(3,2) = inv_far - inv_near;
	if constexpr(!z_zero_one &&  reversed_z) 
		M(3,2) = 0.5f * (inv_near - inv_far);
	if constexpr(!z_zero_one && !reversed_z) 
		M(3,2) = 0.5f * (inv_far - inv_near);

	/* col 3 */
	M(0,3) = shift_x * inv_aspect_x;
	if constexpr( reversed_y) M(1,3) = -shift_y * inv_aspect_y;
	if constexpr(!reversed_y) M(1,3) = +shift_y * inv_aspect_y;
	M(2,3) = -1.f;
	if constexpr( z_zero_one &&  reversed_z) 
		M(3,3) = inv_far;
	if constexpr( z_zero_one && !reversed_z) 
		M(3,3) = inv_near;
	if constexpr(!z_zero_one &&  reversed_z) 
		M(3,3) = 0.5 * (inv_near + inv_far);
	if constexpr(!z_zero_one && !reversed_z) 
		M(3,3) = 0.5 * (inv_near + inv_far);

	return (M);
}

Mat4 orthographic_matrix(const CameraFrustum& f)
{
	assert(f.near != f.far);

	const float aspect_x = f.aspect_x;
	const float aspect_y = f.aspect_y;
	const float shift_x = f.shift_x;
	const float shift_y = f.shift_y;
	const float near = f.near;
	const float far = f.far;

	Mat4 M;

	/* col 0 */
	M(0,0) = aspect_x;
	M(1,0) = 0.f;
	M(2,0) = 0.f;
	M(3,0) = 0.f;

	/* col 1 */
	M(0,1) = 0.f;
	if constexpr( reversed_y) M(1,1) = -aspect_y;
	if constexpr(!reversed_y) M(1,1) = +aspect_y;
	M(2,1) = 0.f;
	M(3,1) = 0.f;

	/* col 2*/
	M(0,2) = 0.f;
	M(1,2) = 0.f;
	if constexpr( z_zero_one &&  reversed_z) 
		M(2,2) = 1.0f / (far - near);
	if constexpr( z_zero_one && !reversed_z) 
		M(2,2) = -1.0f / (far - near);
	if constexpr(!z_zero_one &&  reversed_z) 
		M(2,2) = 2.0f / (far - near);
	if constexpr(!z_zero_one && !reversed_z) 
		M(2,2) = -2.0f / (far - near);
	M(3,2) = 0.f;

	/* col 3 */
	M(0,3) = -shift_x;
	if constexpr( reversed_y) M(1,3) = +shift_y;
	if constexpr(!reversed_y) M(1,3) = -shift_y;
	if constexpr( z_zero_one &&  reversed_z) 
		M(2,3) = 1.0f / (1.0f - near / far);
	if constexpr( z_zero_one && !reversed_z) 
		M(2,3) = -near / (far - near);
	if constexpr(!z_zero_one &&  reversed_z) 
		M(2,3) = (1.0f + near / far) / (1.0f - near / far);
	if constexpr(!z_zero_one && !reversed_z) 
		M(2,3) = -(1.0f + near / far) / (1.0f - near / far);
	M(3,3) = 1.f;
	
	return (M);
}

Mat4 orthographic_matrix_inv(const CameraFrustum& f)
{
	assert(f.near != f.far);

	const float inv_aspect_x = 1.f / f.aspect_x;
	const float inv_aspect_y = 1.f / f.aspect_y;
	const float shift_x = f.shift_x;
	const float shift_y = f.shift_y;
	const float near = f.near;
	const float far = f.far;

	Mat4 M;

	/* col 0 */
	M(0,0) = inv_aspect_x;
	M(1,0) = 0.f;
	M(2,0) = 0.f;
	M(3,0) = 0.f;

	/* col 1 */
	M(0,1) = 0.f;
	if constexpr( reversed_y) M(1,1) = -inv_aspect_y;
	if constexpr(!reversed_y) M(1,1) = +inv_aspect_y;
	M(2,1) = 0.f;
	M(3,1) = 0.f;

	/* col 2*/
	M(0,2) = 0.f;
	M(1,2) = 0.f;
	if constexpr( z_zero_one &&  reversed_z) 
		M(2,2) = far - near;
	if constexpr( z_zero_one && !reversed_z) 
		M(2,2) = near - far;
	if constexpr(!z_zero_one &&  reversed_z) 
		M(2,2) = 0.5f * (far - near);
	if constexpr(!z_zero_one && !reversed_z) 
		M(2,2) = 0.5f * (near - far);
	M(3,2) = 0.f;

	/* col 3 */
	M(0,3) = shift_x * inv_aspect_x;
	if constexpr( reversed_y) M(1,3) = -shift_y * inv_aspect_y;
	if constexpr(!reversed_y) M(1,3) = shift_y * inv_aspect_y;
	if constexpr( z_zero_one &&  reversed_z) 
		M(2,3) = -far;
	if constexpr( z_zero_one && !reversed_z) 
		M(2,3) = -near;
	if constexpr(!z_zero_one &&  reversed_z) 
		M(2,3) = -0.5f * (near + far);
	if constexpr(!z_zero_one && !reversed_z) 
		M(2,3) = -0.5f * (near + far);
	M(3,3) = 1.f;
	
	return (M);
}
