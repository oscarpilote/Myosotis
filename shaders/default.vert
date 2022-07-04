#version 430 core

/* In variables */
layout (location = 0) in vec3 _pos;
layout (location = 1) in vec3 _nml;
layout (location = 2) in vec2 _tex;
layout (location = 3) in int parent_idx;

/* SSBO */
layout(std430, binding = 1) restrict readonly buffer positions {float Pos[];};
layout(std430, binding = 2) restrict readonly buffer normals   {float Nml[];};


/* Uniform variables (cell independent) */
layout (location = 0) uniform mat4 vm;
layout (location = 1) uniform mat4 proj;
layout (location = 2) uniform vec3 camera_pos;
layout (location = 3) uniform bool continuous_lod;
layout (location = 4) uniform bool smooth_shading;
layout (location = 6) uniform float kappa_times_step;

/* Uniform variables (cell dependent) */
layout (location = 7) uniform int  level;
layout (location = 8) uniform int  vtx_offset;
layout (location = 9) uniform int  parent_vtx_offset;


/* Out variables */
layout (location = 0) out vec3 N;  /* Normal vector */
layout (location = 1) out vec3 V;  /* View vector   */
layout (location = 2) out vec3 L;  /* Light vector  */
layout (location = 3) out float ratio;  /* Morphing param  */

float norminf(vec3 v)
{
	return max(max(abs(v.x), abs(v.y)), abs(v.z));
}

void main() 
{
	vec3 pos = _pos;
	vec3 nml = _nml;
	ratio = 0.0;
	if (continuous_lod)
	{
		float l = log2(norminf(camera_pos - pos) / kappa_times_step);
		ratio = smoothstep(level + 0.3, level + 0.7, l);
		uint j = 3 * (parent_idx +  parent_vtx_offset);
		pos = (1. - ratio) * _pos + ratio * vec3(Pos[j + 0], Pos[j + 1], Pos[j + 2]);

		if (smooth_shading)
		{
			nml = (1. - ratio) * nml + ratio * vec3(Nml[j + 0], Nml[j + 1], Nml[j + 2]);
		}
	}

	N = nml;
	V = camera_pos - pos;
	L = V;
	gl_Position = proj * vm * vec4(pos, 1.0f);
}
