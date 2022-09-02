#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 textureCoord;// 2d might not be needed
layout (location = 2) in vec3 posOffset;
layout (location = 4) in vec3 texturePos3d; // 3d
layout (location = 5) in int textureId;
layout (location = 6) in uint faces;

out vec3 texCoord;
out int  texId;
out	vec3 vertexPos;
out vec3 vertexPosFromCamera;
flat out int shouldBeDiscarded;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 precalcMat;

void main()
{
	texCoord = texturePos3d;
	texId = textureId;

	// each cube has an integer that tells us if its faces should be rendered or not
	// it has one byte set for each face (see order in rectangularCuboid.cpp
	// that bit is updaed every 6 calls because each face is composed of 2 triangles (2 * 3 points)
	if ((faces & (1 << (gl_VertexID / 6))) != 0)
	{
		gl_Position = precalcMat * vec4(vec3(pos + posOffset), 1.0);
	}
	else
	{
		gl_Position = vec4(0, 0, -1, 1);// render triangle outside the screen if the face should'nt be rendered
		return;
	}

	vertexPos = vec3(pos + posOffset);
	vertexPosFromCamera = vec3(gl_Position);
}
