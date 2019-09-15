uniform mat4 u_projection;

attribute vec2 a_position;
attribute vec2 a_uv;
attribute vec4 a_color;

varying vec2 v_uv;
varying vec4 v_color;

void main()
{
	v_uv = a_uv;
	v_color = a_color;
	gl_Position = u_projection * vec4(a_position.xy,0,1);
}