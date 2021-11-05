#version 450 core

out vec4 FragColor;

in vec3	vertexPos;
in vec3 texCoord;
in vec3 vertexPosFromCamera;

uniform samplerCube texture0;

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
	float w = ceil(vertexPos.y) / 64.0;
	if (ceil(vertexPos.y) <= 1.0)
		FragColor = vec4(76.0 / 255.0, 70.0 / 255.0, 50.0 / 255.0, 1.0);
	else if (w != 1)
	{
		w *= interpolate(vertexPosFromCamera.z, 70.f, 140.f);
		FragColor = vec4(0, w, 0, 1.0);
	}
	else
		FragColor = vec4(1, 0, 0, 1.0); // show in red to debug
	//FragColor = texture(texture0, texCoord);
}
