#version 460 core

// In variables
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nml;
layout (location = 2) in vec2 tex;


// Uniform variables
layout (location = 0) uniform mat4 vm;
layout (location = 1) uniform mat4 proj;
layout (location = 2) uniform vec3 camera_pos;


// Out variables
layout (location = 0) out vec3 N;  /* Normal vector */
layout (location = 1) out vec3 L;  /* Light vector  */
layout (location = 2) out vec3 V;  /* View vector   */

void main() {

	N = normalize(nml);
	L = normalize(vec3(1.f, 3.f, 1.f));
	V = normalize(camera_pos - pos);
	gl_Position = proj * vm * vec4(pos, 1.0f);
}
