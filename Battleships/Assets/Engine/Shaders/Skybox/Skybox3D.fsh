varying vec3 v_uv;

uniform samplerCube s_texture;
uniform float u_opacity;

void main()
{    
    gl_FragColor = textureCube(s_texture, v_uv);
	gl_FragColor *= u_opacity;
}