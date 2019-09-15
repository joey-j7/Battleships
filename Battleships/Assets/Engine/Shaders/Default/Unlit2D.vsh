attribute vec3 a_position;
attribute vec2 a_uv;

uniform mat4 u_orthoModel;

varying vec2 v_uv;

void main()
{
	v_uv = a_uv;
    gl_Position = u_orthoModel * vec4(a_position.xz, 0, 1);
}