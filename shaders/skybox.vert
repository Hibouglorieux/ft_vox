#version 450 core
layout (location = 0) in vec3 aPos;

out vec3 pos;
out vec3 texCoord;

uniform mat4 precalcMat;

void main()
{
	texCoord = aPos;
	gl_Position = precalcMat * vec4(aPos * 800 , 1.0);
	pos = aPos;
}
