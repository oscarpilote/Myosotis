#version 430 core

/* In variables */
layout(std430, binding = 1) restrict readonly buffer positions {float Pos[];};
layout(std430, binding = 2) restrict readonly buffer normals   {float Nml[];};



/* Uniform variables */
layout (location = 0) uniform mat4 vm;
layout (location = 1) uniform mat4 proj;
layout (location = 2) uniform vec3 camera_pos;


/* Out variables */
layout (location = 0) out vec3 N;  /* Normal vector */
layout (location = 1) out vec3 V;  /* View vector   */
layout (location = 2) out vec3 L;  /* Light vector  */

void main() 
{
	uint i = gl_VertexID;
	vec3 pos = vec3(Pos[3 * i + 0], Pos[3 * i + 1], Pos[3 * i + 2]);
	vec3 nml = vec3(Nml[3 * i + 0], Nml[3 * i + 1], Nml[3 * i + 2]);

	N = nml;
	V = camera_pos - pos;
	L = V;
	gl_Position = proj * vm * vec4(pos, 1.0f);
}
