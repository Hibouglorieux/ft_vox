#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 8) in vec3 aNormal;
layout (location = 2) in vec3 posOffset;
layout (location = 4) in vec3 texturePos3d; // 3d
											
layout (location = 5) in float textureId;
layout (location = 6) in float face;

out vec3 texCoord;
out int  texId;
out	vec3 vertexPos;
out vec3 vertexPosFromCamera;
out vec3 normal;
out vec4 FragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 precalcMat;
uniform vec2 playerPos;
uniform mat4 lightSpaceMatrix;

void main()
{
	texCoord = texturePos3d;
	texId = int(textureId);
	vec2 pos2d = vec2(posOffset.x - playerPos.x, posOffset.z - playerPos.y);

	// each cube has an integer that tells us if its faces should be rendered or not
	// it has one byte set for each face (see order in rectangularCuboid.cpp
	// that bit is updaed every 6 calls because each face is composed of 2 triangles (2 * 3 points)
	if (((uint(face) & (1 << (gl_VertexID / 6))) != 0) && (length(pos2d) < 160))
		gl_Position = precalcMat * vec4(vec3(pos + posOffset), 1.0);
	else
		gl_Position = vec4(-1, -1, -1, 0);// render triangle outside the screen if the face should'nt be rendered

	vertexPos = vec3(pos + posOffset);
	vertexPosFromCamera = vec3(gl_Position);
	FragPosLightSpace = lightSpaceMatrix * vec4(vertexPos, 1.0) * precalcMat;
	normal = aNormal;//transpose(inverse(mat3(projection))) * aNormal;
}
