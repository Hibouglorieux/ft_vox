#version 450 core

out vec4 FragColor;

in vec3	vertexPos;
in vec3 texCoord;
in vec3 vertexPosFromCamera;
flat in int texId;

uniform bool color = false;

uniform samplerCube[8] allTextures;

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
	/*
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
	*/
		FragColor = texture(allTextures[texId], texCoord);
}
