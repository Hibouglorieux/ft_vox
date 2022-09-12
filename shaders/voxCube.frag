#version 450 core

out vec4 FragColor;

in vec3 normal;
in vec3	vertexPos;
in vec3 texCoord;
in vec3 vertexPosFromCamera;
in vec4 FragPosLightSpace;
flat in int texId;

uniform bool color = false;

uniform samplerCube[8] allTextures;
uniform sampler2D shadowMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

float interpolate(float value, float minimum, float maximum)
{
	float retVal;
	retVal = (value - minimum) / maximum;

	return 1.0f - max(min(retVal, 1.0f), 0.0f);
}

void main()
{
	// Texture

	// Gradient color
	if (color)
	{
		float w = ceil(vertexPos.y) / 512.0 + 0.5;
		if (w >= 0.0 && w <= 1.0)
		{
			//w *= interpolate(vertexPosFromCamera.z, 70.f, 140.f);
			FragColor = vec4(0, w, 0, 1.0);
		}
		else if (w > 1.0)
			FragColor = vec4(1, 0, 0, 1.0); // show in red to debug
		else if (w < 0.0)
			FragColor = vec4(1, 0, 1, 1.0); // show in red to debug
	}
	else
		FragColor = texture(allTextures[texId], texCoord);

	// ambient
    float ambientStrength = 0.25;
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);
  	
    // diffuse 
	vec3 norm = normalize(normal);
    vec3 lightDir = normalize(vec3(0, 512, 0) - vertexPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

	float shadow = 0.0;//ShadowCalculation(FragPosLightSpace);       
    vec3 lighting = (ambient + (1.0 - shadow) * diffuse) * FragColor.xyz;

    FragColor = vec4(lighting, 1);
}
