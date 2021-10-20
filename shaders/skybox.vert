#version 450 core

layout (location = 0) in vec3 pos;

out vec3 texCoord;
out	vec3 vertexPos;

uniform mat4 precalcMat;

void main()
{
	texCoord = pos;
	gl_Position = precalcMat * vec4(pos * 800 , 1.0);
}
