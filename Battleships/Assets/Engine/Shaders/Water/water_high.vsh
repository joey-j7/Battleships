attribute vec3 a_position;
attribute vec3 a_normal;

varying vec3 v_position;  
varying vec3 v_normal;  

uniform vec3 u_viewPos;

uniform mat4 u_model;
uniform mat3 u_normal;
uniform mat4 u_pv;

void main()
{	
    v_position = vec3(u_model * vec4(a_position, 1.0));
    v_normal = u_normal * a_normal;  
	
	gl_Position = u_pv * u_model * vec4(a_position, 1.0);
} 