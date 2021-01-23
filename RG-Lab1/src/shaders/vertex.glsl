#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texCoord;

layout (location = 0) out vec2 fragTexCoord;
layout (location = 1) out vec3 fragNorm;
layout (location = 2) out vec3 fragWorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	fragTexCoord = texCoord;
	fragNorm = normalize((model * vec4(norm, 0)).xyz);
	vec4 worldPos = model * vec4(pos, 1);
	fragWorldPos = worldPos.xyz;
	gl_Position = projection * view * worldPos;
}