#version 450 core
layout (location = 0) in vec3 aPos;

out vec3 pos;
out vec3 texCoord;

uniform mat4 precalcMat;
uniform vec3 playerPos;

void main()
{
	//vec3 gPos = vec3(aPos.x + playerPos.x, aPos.y, playerPos.z + aPos.z);
	vec3 gPos = aPos;

	texCoord = aPos;
	gl_Position = precalcMat * vec4(gPos * 800 + playerPos, 1.0);
	pos = aPos;
}
