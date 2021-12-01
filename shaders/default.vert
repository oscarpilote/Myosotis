#version 430 core

/* In variables */
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nml;
layout (location = 2) in vec2 tex;


/* Uniform variables */
layout (location = 0) uniform mat4 vm;
layout (location = 1) uniform mat4 proj;
layout (location = 2) uniform vec3 camera_pos;


/* Out variables */
layout (location = 0) out vec3 N;  /* Normal vector */
layout (location = 1) out vec3 V;  /* View vector   */
layout (location = 2) out vec3 L;  /* Light vector  */

void main() {

	N = nml;
	V = camera_pos - pos;
	L = V;
	gl_Position = proj * vm * vec4(pos, 1.0f);
}
