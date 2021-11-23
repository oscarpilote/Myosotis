#version 460 core

// In variables
layout (location = 0) in vec3 N;  /* Normal vector */
layout (location = 1) in vec3 L;  /* Light vector  */
layout (location = 2) in vec3 V;  /* View vector   */

// Out color
layout (location = 0) out vec4 color;

#define AMBIENT_COLOR vec3(0.2f, 0.1f, 0.0f)
#define Ka 1.0f
#define DIFFUSE_COLOR (0.5 * (N + 1))
#define Kd 1.0f
#define SPECULAR_COLOR vec3(.8f, .8f, 1.f)
#define Ks 1.0f
#define shininess 80


// Main function
void main() {
	float Id = max(dot(N, L), 0.f);
	float Is = 0.f;
	if (Id > 0)
	{
		//vec3 R = 2.f * dot(N, L) * N - L;
		vec3 R = reflect(-L, N);
		float ca = max(dot(R, V), 0.f); 
		Is = pow(ca, shininess);
	}
	vec3 full = Ka * AMBIENT_COLOR;
	full += Kd * Id * DIFFUSE_COLOR;
	full += Ks * Is * SPECULAR_COLOR; 
	color = vec4(full, 1.0f);
}
