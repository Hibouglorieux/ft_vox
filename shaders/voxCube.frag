#version 450 core

out vec4 FragColor;

in vec3	vertexPos;
in vec3 texCoord;

uniform samplerCube texture0;

void main()
{
	// Texture

	// Gradient color
	float w = ceil(vertexPos.y) / 64.0;
	if (ceil(vertexPos.y) <= 1.0)
		FragColor = vec4(76.0 / 255.0, 70.0 / 255.0, 50.0 / 255.0, 1.0);
	else
		FragColor = vec4(0, w, 0, 1.0);
	//FragColor = texture(texture0, texCoord);
}
