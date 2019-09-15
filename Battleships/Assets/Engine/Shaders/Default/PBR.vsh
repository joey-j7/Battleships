attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_uv;

varying vec3 v_position;
varying vec2 v_uv;
varying vec3 v_normal;

uniform mat4 u_pv;
uniform mat4 u_model;

void main()
{
    v_uv = a_uv;
    v_position = vec3(u_model * vec4(a_position, 1.0));
    v_normal = mat3(u_model) * a_normal;   

    gl_Position = u_pv * vec4(v_position, 1.0);
}