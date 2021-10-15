#version 450 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 textureCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 precalcMat;
out vec2 texCoord;

void main()
{
	texCoord = textureCoord;
    gl_Position = precalcMat * vec4(pos, 1.0);
}
