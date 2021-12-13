#version 430 core

/* Uniform variables */
layout (location = 0) uniform mat4 vm;
layout (location = 1) uniform mat4 proj;
layout (location = 2) uniform vec3 camera_pos;

layout(std430, binding = 0) restrict readonly buffer Indices   {uint indices[];};
layout(std430, binding = 1) restrict readonly buffer Positions {float positions[];};

layout (location = 0) out vec3 nml;

void main(void) 
{

	uint tri_idx = gl_VertexID / 2;
	bool end_pt  = (gl_VertexID & 1) == 1;

	uint idx1 = indices[3 * tri_idx + 0];
	uint idx2 = indices[3 * tri_idx + 1];
	uint idx3 = indices[3 * tri_idx + 2];

	vec3 v1 = vec3(positions[3 * idx1 + 0],positions[3 * idx1 + 1],
			positions[3 * idx1 + 2]);
	vec3 v2 = vec3(positions[3 * idx2 + 0],positions[3 * idx2 + 1],
			positions[3 * idx2 + 2]);
	vec3 v3 = vec3(positions[3 * idx3 + 0],positions[3 * idx3 + 1],
			positions[3 * idx3 + 2]);
	
	//vec3 v1 = positions[idx1];
	//vec3 v2 = positions[idx2];
	//vec3 v3 = positions[idx3];


	vec3 v = (v1 + v2 + v3) * 0.33333333f;

	float dist = length(camera_pos - v);

	nml = normalize(cross(v2 - v1, v3 - v1));

	if (end_pt)
	{
		v += 0.03 * dist * nml;
	}

	gl_Position = proj * vm * vec4(v, 1.f);
}
