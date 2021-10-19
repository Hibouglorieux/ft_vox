#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 textureCoord;
layout (location = 2) in vec3 posOffset;

out vec2 texCoord;
out	vec3 vertexPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 precalcMat;
uniform mat4 model; // Should be useless from now on

uniform bool instanced;

void main()
{
	texCoord = textureCoord;

	if (instanced)
	{
		gl_Position = precalcMat * vec4(vec3(pos + posOffset), 1.0);
	}
	else
	{
		gl_Position = projection * view * model * vec4(pos, 1.0);
	}
	vertexPos = vec3(pos + posOffset);
}
