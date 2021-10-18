#version 450 core
out vec4 FragColor;
in vec2 texCoord;
uniform sampler2D texture0;

void main()
{
	float colo = texture(texture0, texCoord).r;
	FragColor = vec4(colo, colo, colo, 1.0);
}
