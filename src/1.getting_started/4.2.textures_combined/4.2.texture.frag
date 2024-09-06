#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D sampler2D1;
uniform sampler2D sampler2D2;

void main()
{
	// linearly interpolate between both textures (80% container, 20% awesomeface)
	FragColor = mix(texture(sampler2D1, TexCoord), texture(sampler2D2, TexCoord), 0.2);
}