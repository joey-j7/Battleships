uniform float u_opacity;
uniform vec3 u_color;

uniform sampler2D s_texture;

varying vec2 v_uv;
                  
void main()                                                    
{                                                              
	gl_FragColor = texture2D(s_texture, v_uv) * vec4(u_color, 1.0);
	gl_FragColor.a *= u_opacity;
}