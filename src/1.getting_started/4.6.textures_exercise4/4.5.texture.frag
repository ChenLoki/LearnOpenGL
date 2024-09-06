#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float mixValue;

// texture samplers
uniform sampler2D texturea;
uniform sampler2D textureb;

void main()
{
	// linearly interpolate between both textures
	FragColor = mix(texture(texturea, TexCoord), texture(textureb, TexCoord), mixValue);
}