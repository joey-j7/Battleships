uniform samplerCube s_skyTexture;

varying vec3 v_reflectionDir;

uniform vec4 u_waterColor;
uniform float u_opacity;

void main() {
	/* Get reflection */
	vec4 reflection = textureCube(s_skyTexture, v_reflectionDir);
    gl_FragColor = vec4(mix(vec3(reflection), vec3(u_waterColor), u_waterColor.a), 1.0);
	gl_FragColor.a = 0.8;
	gl_FragColor *= u_opacity;
}