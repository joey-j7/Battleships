attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_uv;

uniform mat4 u_mvp;

varying vec2 v_uv;

void main()
{
	v_uv = a_uv;
    gl_Position = u_mvp * vec4(a_position, 1.0);
}