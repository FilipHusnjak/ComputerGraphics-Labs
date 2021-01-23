#version 460 core

layout (location = 0) in vec2 fragTexCoords;
layout (location = 1) in vec3 fragPos;
layout (location = 2) in vec3 fragParticleSystemCenter;

out vec4 outColor;

uniform sampler2D tex;

void main()
{
	vec3 toCenter = fragPos - fragParticleSystemCenter;
	float distToCentreSquared = pow(toCenter.x, 2) + pow(toCenter.y, 2) + pow(toCenter.z, 2);
	outColor = texture(tex, fragTexCoords);

	outColor.b /= clamp(distToCentreSquared * 2, 0, 1);
	outColor.g /= clamp(distToCentreSquared * 2, 0, 1);

	float maxDist = 15.f;
	vec4 smokeColor = vec4(0.2, 0.2, 0.2, 1);
	float k = min((distToCentreSquared / maxDist), 1);
	outColor.r = outColor.r + (smokeColor.r - outColor.r) * k;
	outColor.g = outColor.g + (smokeColor.g - outColor.g) * k;
	outColor.b = outColor.b + (smokeColor.b - outColor.b) * k;
	outColor.a -= distToCentreSquared / 1000.f;
	outColor.a = clamp(outColor.a, 0, 1);
}  