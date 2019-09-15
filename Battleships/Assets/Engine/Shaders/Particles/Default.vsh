// Attributes
attribute float a_id;
attribute float a_radiusOffset;
attribute float a_velocityOffset;
attribute float a_sizeOffset;

// Uniforms
uniform mat4 u_mvp;

uniform float u_time;
uniform float u_radius;
uniform float u_velocity;
uniform float u_size;

uniform vec3 u_offset;
uniform float u_camDist;

void main()
{
	// 1
	// Convert polar angle to cartesian coordinates and calculate radius
	float x = cos(a_id);
	float y = sin(a_id);
	float r = u_radius * a_radiusOffset;

	// 2
	// Lifetime
	float time = u_time * (u_velocity + a_velocityOffset);
	x = x * r * time;
	y = y * r * time;

	// 5
	// Required OpenGLES 2.0 outputs
	gl_Position = u_mvp * vec4(vec3(0.0, y, x) + u_offset, 1.0);
	gl_PointSize = max(0.0, (u_size + a_sizeOffset) / u_camDist);
}