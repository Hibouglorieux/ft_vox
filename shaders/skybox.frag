
#version 450 core

out vec4 FragColor;

in vec3 texCoord;

uniform samplerCube skyboxTexture;

void main()
{
	float r = texture(skyboxTexture, texCoord).r;

	FragColor = vec4(r, r, r, 1.0);
}
