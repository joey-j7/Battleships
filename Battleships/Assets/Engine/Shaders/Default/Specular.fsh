varying vec2 v_uv;
varying float v_intensity;

uniform float u_opacity;
uniform sampler2D s_texture;
	
void main()
{
	// add the specular term 
    gl_FragColor = v_intensity * texture2D(s_texture, v_uv);
	gl_FragColor.a *= u_opacity;
}