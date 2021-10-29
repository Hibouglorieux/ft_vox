
#version 450 core

out vec4 FragColor;

in vec3 texCoord;

uniform samplerCube texture0;

void main()
{
	float r = texture(texture0, texCoord).r;

	FragColor = vec4(r, r, r, 1.0);
}
