#version 330 core
layout (location = 0) in vec3 pos;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 precalcMat;
out mat4 invView;

void main()
{
    gl_Position = precalcMat * vec4(pos, 1.0);
	invView = inverse(view);
}
