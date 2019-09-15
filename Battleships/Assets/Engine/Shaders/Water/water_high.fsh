struct LightProperties {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform samplerCube s_skyTexture;
uniform vec3 u_viewPos;

uniform float u_heightMax = 0;
uniform float u_heightMin = 0;

uniform vec3 u_lightPosition;

varying vec3 v_position;  
varying vec3 v_normal;  

LightProperties u_lightProperties;

void main()
{
	u_lightProperties.ambient = vec3(1.0, 1.0, 1.0);
	u_lightProperties.diffuse = vec3(1.0, 1.0, 1.0);
	u_lightProperties.specular = vec3(1.0, 0.9, 0.7);

	vec3 norm = normalize(v_normal);
	vec3 lightDir = normalize(u_lightPosition - v_position); 
	vec3 viewDir = normalize(u_viewPos - v_position);
	
	vec3 ambientFactor = vec3(0.0);
	vec3 diffuseFactor = vec3(1.0);
	
	vec3 skyColor = vec3(0.45, 0.70, 0.75);
	
	if (dot(norm, viewDir) < 0) norm = -norm;
	
    // Ambient
    vec3 ambient = u_lightProperties.ambient * ambientFactor;
	
	// Height Color
	vec3 shallowColor = vec3(0.0, 0.54, 0.3);
	vec3 deepColor = vec3(0.02, 0.05, 0.10);
	
	float relativeHeight;	// from 0 to 1
	relativeHeight = (v_position.y - u_heightMin) / (u_heightMax - u_heightMin);
	vec3 heightColor = relativeHeight * shallowColor + (1 - relativeHeight) * deepColor;
	// heightColor = vec3(s);	// Black and white
	
	// Spray
	float sprayThresholdUpper = 1.0;
	float sprayThresholdLower = 0.9;
	float sprayRatio = 0;
	if (relativeHeight > sprayThresholdLower) sprayRatio = (relativeHeight - sprayThresholdLower) / (sprayThresholdUpper - sprayThresholdLower);
	vec3 sprayBaseColor = vec3(1.0);
	vec3 sprayColor = sprayRatio * sprayBaseColor;	
	
    // Diffuse  	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diffuseFactor * u_lightProperties.diffuse * diff;
	diffuse = vec3(0.0);
	
	// Psudo reflect
	float refCoeff = pow(max(dot(norm, viewDir), 0.0), 0.3);	// Smaller power will have more concentrated reflect.
	vec3 reflectColor = (1 - refCoeff) * skyColor;
	
    // Specular
	//vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(norm, halfwayDir), 0.0), 64.0);
	vec3 reflectDir = reflect(-lightDir, norm); 
	float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0), 64) * 3;	// Over exposure
    vec3 specular = u_lightProperties.specular * specCoeff;
        
	vec3 reflection = vec3(textureCube(s_skyTexture, viewDir));
	vec4 combinedColor = vec4(mix(ambient + diffuse + heightColor + reflectColor, reflection, 0.2), 0.8);    
	
	//sprayRatio = clamp(sprayRatio, 0, 1);
	//combinedColor *= (1 - sprayRatio);
	//combinedColor += sprayColor;
	
	specCoeff = clamp(specCoeff, 0, 1);
	combinedColor *= (1 - specCoeff);
	combinedColor += vec4(specular, 1.0 * specCoeff);	
	gl_FragColor = combinedColor;
	
	//gl_FragColor = vec4(sprayColor, 1.0); 
	//gl_FragColor = vec4(heightColor, 1.0); 
	//gl_FragColor = vec4(specular, 1.0); 
	//gl_FragColor = vec4(reflection, 1.0);
	//gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);
} 