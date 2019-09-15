attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_uv;

varying vec2 v_position;
varying vec3 v_reflectionDir;

uniform mat4 u_pv;
uniform mat4 u_model;

uniform vec2 u_waveOffset;
uniform vec3 u_camPos;

const float waveSpeed = 10.0;
const float waveHeight = 0.2;

float tiling = 0.05;

#define PI 3.1415926535897932384626433832795

void main() {
	/* Move X and Y along camera for endless water */
	vec4 position = u_model * vec4(a_position, 1.0);
	position = vec4(u_camPos.x + position.x, position.y, u_camPos.z + position.z, 1.0);
	
	/* Animate wave height */
	position.y += u_waveOffset.y;
 
	v_position = position.xz * tiling;
	
	vec3 normal = normalize(vec3(u_model * vec4(a_normal, 0.0)));
	vec3 viewDir = normalize(position.xyz - u_camPos);
	v_reflectionDir = reflect(viewDir, -normal);
	
	gl_Position = u_pv * position;
}