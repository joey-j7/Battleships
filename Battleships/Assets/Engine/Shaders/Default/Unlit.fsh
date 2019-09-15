uniform sampler2D s_texture;
uniform float u_opacity;

varying vec2 v_uv;
                  
void main()                                                    
{                                                              
	gl_FragColor = texture2D(s_texture, v_uv);
	gl_FragColor.a *= u_opacity;
}                                                              