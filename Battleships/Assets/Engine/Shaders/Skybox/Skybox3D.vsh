attribute vec3 a_position;

varying vec3 v_uv;

uniform mat4 u_pv;

void main()
{
    v_uv = a_position;
	gl_Position = (u_pv * vec4(a_position, 1.0)).xyww;
	gl_Position.z -= 0.00001;
}