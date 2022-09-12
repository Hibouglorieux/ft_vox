#version 450 core

out vec4 FragColor;

in vec3 normal;
in vec3	vertexPos;
in vec3 texCoord;
in vec3 vertexPosFromCamera;
in vec4 FragPosLightSpace;
flat in int texId;

uniform bool color = false;

uniform samplerCube[8] allTextures;
uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

float near = 0.1;
float far  = 128.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main()
{             
    float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
    FragColor = vec4(vec3(depth), 1.0);
}