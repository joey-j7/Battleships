uniform samplerCube s_skyTexture;
uniform sampler2D s_dudvTexture;

varying vec2 v_position;
varying vec3 v_reflectionDir;

uniform float u_heightDistance;

uniform vec2 u_waveOffset;
uniform vec4 u_waterColor;

uniform float u_opacity;

float waveStrength = 0.05;

void main() {
	/* Dynamic wave strength when close or far */
	waveStrength += (u_heightDistance - 10.0) * waveStrength * 0.01;
	
	/* Get distortion from DUDV */
	vec2 distortionTex = texture2D(s_dudvTexture, vec2(v_position.x + u_waveOffset.x, v_position.y)).rg * 0.07;
	distortionTex = v_position + vec2(distortionTex.x, distortionTex.y + u_waveOffset.x);
	vec2 distortion = (texture2D(s_dudvTexture, distortionTex).rg * 2.0 - 1.0) * waveStrength;

	/* Get reflection */
	vec3 reflected = v_reflectionDir;
	reflected += vec3(distortion.x, 0, distortion.y);
	vec4 reflection = textureCube(s_skyTexture, reflected);
	
	// /* Get specular from normal map */
	// // vec4 normalColor = texture2D(s_normalTexture, distortionTex);
	// // vec3 normal = normalize(vec3(normalColor.r * 2.0 - 1.0, normalColor.b, normalColor.g * 2.0 - 1.0));
	
	// // /* Get reflected specular lighting */
	// // vec3 reflectedLight = reflect(lightDir, normal);
	// // float specular = max(dot(reflectedLight, v_cameraDir), 0.0);
	// // specular = pow(specular, shineDamper);
	// // vec3 specularHightlights = lightCol * specular * reflectivity;
	
    gl_FragColor = vec4(mix(vec3(reflection), vec3(u_waterColor), u_waterColor.a), 1.0);
	gl_FragColor.a = 0.8;
	gl_FragColor *= u_opacity;
}