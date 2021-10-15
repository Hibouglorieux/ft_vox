#version 330 core
out vec4 FragColor;
uniform vec3 myColor;
in mat4 invView;

void main()
{
	if (invView[3][1] < 0.1f)
		discard;
	FragColor = vec4(myColor, 1.0);
}
