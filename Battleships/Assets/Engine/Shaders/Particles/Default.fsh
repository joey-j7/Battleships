// Uniforms
uniform highp float u_time;
uniform highp vec3 u_color;

uniform float u_opacity;

uniform sampler2D s_texture;

void main()
{
	// Color
	highp vec4 color = vec4(1.0);
	color.rgb = u_color;
	color.rgb = clamp(color.rgb, vec3(0.0), vec3(1.0));
	
	highp vec4 texture = texture2D(s_texture, gl_PointCoord);
	gl_FragColor = texture * color;
	gl_FragColor.a *= u_opacity;
}