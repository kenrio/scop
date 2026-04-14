#version 330 core

out vec4	FragColor;

in vec3	ourColor;
in vec2	texCoord;

uniform sampler2D	ourTexture;
uniform float		mixValue;

void main()
{
	vec4	color = vec4(ourColor, 1.0);
	vec4	tex = texture(ourTexture, texCoord);
	FragColor = mix(color, tex, mixValue);
}
