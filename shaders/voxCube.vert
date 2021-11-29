#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 textureCoord;// 2d might not be needed
layout (location = 2) in vec3 posOffset;
layout (location = 4) in vec3 texturePos3d; // 3d
layout (location = 5) in int textureId;

out vec3 texCoord;
out int  texId;
out	vec3 vertexPos;
out vec3 vertexPosFromCamera;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 precalcMat;

void main()
{
	texCoord = texturePos3d;
	texId = textureId;

	gl_Position = precalcMat * vec4(vec3(pos + posOffset), 1.0);

	vertexPos = vec3(pos + posOffset);
	vertexPosFromCamera = vec3(gl_Position);
}
