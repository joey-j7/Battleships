// Uniforms
uniform highp float u_time;
uniform highp vec3 u_color;

uniform float u_opacity;

void main()
{
	// Color
	highp vec4 color = vec4(1.0);
	color.rgb = u_color;
	color.rgb = clamp(color.rgb, vec3(0.0), vec3(1.0));
	
	gl_FragColor = color;
	gl_FragColor.a *= u_opacity;
}