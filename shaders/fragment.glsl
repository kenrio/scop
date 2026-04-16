#version 330 core

out vec4	FragColor;

in vec3	ourColor;
in vec2	texCoord;
in vec3	normal;

uniform sampler2D	ourTexture;
uniform float		mixValue;
uniform float		lightingValue;

void main()
{
	vec3	lightDir = normalize(vec3(0.5, 1.0, 0.8));
	vec3	norm = normalize(normal);
	float	diff = max(dot(norm, lightDir), 0.0);
	float	ambient = 0.2;
	float	dynamicLighting = ambient + diff * 0.8;
	float	lighting = mix(1.0, dynamicLighting, lightingValue);

	vec4	color = vec4(ourColor * lighting, 1.0);
	vec4	tex = texture(ourTexture, texCoord) * vec4(vec3(lighting), 1.0);
	FragColor = mix(color, tex, mixValue);
}
