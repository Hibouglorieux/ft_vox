#version 450 core
out vec4 FragColor;

in vec3	vertexPos;
in vec2 texCoord;

uniform sampler2D texture0;

void main()
{
	float colo = texture(texture0, texCoord).r;
	FragColor = vec4(colo, colo, colo, 1.0);

	float w = vertexPos.y / 16.0;
	float height = (1.0 - w) * 255 + w * 0;

	FragColor = vec4(0, height, 0, 1.0);
}
