#version 450 core

out vec4 FragColor;

in vec3	vertexPos;
in vec3 texCoord;

uniform samplerCube texture0;

void main()
{
	// Texture
	//FragColor = texture(texture0, texCoord);


	// Gradient color
	
	float w = ceil(vertexPos.y) / 64.0;
	FragColor = vec4(0, w, 0, 1.0);
}
