#version 450 core

out vec4 FragColor;

in vec3	vertexPos;
in vec3 texCoord;
in vec3 vertexPosFromCamera;
flat in int texId;

uniform bool color = false;

uniform samplerCube water;
uniform samplerCube sand;
uniform samplerCube dirt;
uniform samplerCube grass;
uniform samplerCube grass_snow;
uniform samplerCube stone;
uniform samplerCube snow;
uniform samplerCube bedrock;

float interpolate(float value, float minimum, float maximum)
{
	float retVal;
	retVal = (value - minimum) / maximum;

	return 1.0f - max(min(retVal, 1.0f), 0.0f);
}

void main()
{
	int tmp = texId; // TODO wtf this is incorrect
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
	{
		//tmp = tex;
		if (tmp == 0)
			FragColor = texture(water, texCoord);
		else if (tmp == 1)
			FragColor = texture(sand, texCoord);
		else if (tmp == 2)
			FragColor = texture(dirt, texCoord);
		else if (tmp == 3)
			FragColor = texture(grass, texCoord);
		else if (tmp == 4)
			FragColor = texture(grass_snow, texCoord);
		else if (tmp == 5)
			FragColor = texture(stone, texCoord);
		else if (tmp == 6)
			FragColor = texture(snow, texCoord);
		else if (tmp == 7)
			FragColor = texture(bedrock, texCoord);
		else 
			FragColor = vec4(1., 0, 0, 1.0); // show in red to debug
	}
}
