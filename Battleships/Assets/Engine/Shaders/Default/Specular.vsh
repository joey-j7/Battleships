attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_uv;

uniform mat4 u_mvp;
uniform mat4 u_model;

varying vec2 v_uv;
varying float v_intensity;

uniform vec3 u_lightPosition;

void main()
{
	v_uv = a_uv;
	
	/* Calculate lighting */
	vec4 spec = vec4(0.0);
	
	vec4 modelPos = u_model * vec4(a_position, 1.0);
	
	vec3 n = normalize(mat3(u_model) * a_normal);
	vec3 lightDirection = normalize(u_lightPosition - vec3(modelPos));
 
	float distance = 1.0;
    float attenuation = 1.0 / (distance * distance);
    float intensity = max(dot(n, lightDirection), 0.0) * attenuation;
 
	v_intensity = min(max(intensity, 1.0), 1.2);
	
	/* Calculate position */
	gl_Position = u_mvp * vec4(a_position, 1.0);
}