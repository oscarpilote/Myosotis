#version 460 core

// In variables
layout (location = 0) in vec3 _N;  /* Normal vector */
layout (location = 1) in vec3 _V;  /* View vector   */
layout (location = 2) in vec3 _L;  /* Light vector  */

// Uniform variables
layout (location = 3) uniform bool smooth_shading;

// Out color
layout (location = 0) out vec4 color;

#define AMBIENT_COLOR vec3(1.f, 0.8f, 1.f)
#define Ka 0.2f
#define DIFFUSE_COLOR vec3(1.f, 1.f, 1.f)
#define Kd 0.6f
#define SPECULAR_COLOR vec3(.8f, .8f, 1.f)
#define Ks 0.2f
#define shininess 80


void main() {

	vec3 V = normalize(_V);
	vec3 L = normalize(_L);
	vec3 N;
	if (smooth_shading)
	{
		N = normalize(_N);
		if (dot(V, N) < 0)
		{
			N = -N;
		}
	}
	else /* flat shading */
	{
		/**
		 * Yields face normal, using unormalized _V is equiv to 
		 * using world pos but avoids an additional in variable
		 */
		N = normalize(cross(dFdx(_V), dFdy(_V)));
	}

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
