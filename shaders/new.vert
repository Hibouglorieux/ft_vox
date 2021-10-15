#version 330 core
layout (location = 0) in vec3 pos;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 precalcMat;
out vec3 FragPos;

void main()
{
    gl_Position = precalcMat * vec4(pos, 1.0);
    FragPos = vec3(model * vec4(pos, 1.0));
}
