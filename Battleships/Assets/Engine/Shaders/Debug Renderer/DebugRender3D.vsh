attribute vec3 a_position;
attribute vec3 a_debugcolor;

uniform mat4 u_pv;

varying vec3 v_outColor;

void main()
{
	v_outColor = a_debugcolor;
	gl_Position = u_pv * vec4(a_position, 1.0);
}