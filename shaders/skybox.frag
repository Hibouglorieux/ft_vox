
#version 450 core

out vec4 FragColor;

in vec3 pos;
in vec3 texCoord;

uniform samplerCube skyboxTexture;

uniform vec3	sunPos;
uniform float	sunIntensity;
uniform vec4	sunColor;
uniform vec4	moonColor;

void main()
{
	vec3 moonPos = -sunPos;

	float moonIntensity = 1 - sunIntensity;
	float r = texture(skyboxTexture, texCoord).r;

	// the gradient[Sun/Moon] float will be a value between [1, 0]
	float gradientSun  = dot(normalize(sunPos), normalize(pos)) / 2.0 + 0.5;
	float gradientMoon = 1 - (dot(normalize(moonPos), normalize(pos)) / 2.0 + 0.5);

	// We then must mix the sun color with the moon color and using their
	// intensity to offset each other
	vec4 SunSkyColor = mix(moonColor * moonIntensity, sunColor * sunIntensity, smoothstep(0.0, 1.25, gradientSun));
	//vec4 MoonSkyColor = mix(moonColor * moonIntensity, sunColor * sunIntensity, smoothstep(0.0, 1.25, gradientMoon));

	FragColor = SunSkyColor;//mix(SunSkyColor, MoonSkyColor, smoothstep(0.0, 1.0, moonIntensity));
}
