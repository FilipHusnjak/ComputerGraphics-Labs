#version 460 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 center;
layout (location = 3) in vec3 color;

layout (location = 0) out vec2 fragTexCoords;
layout (location = 1) out vec3 fragPos;
layout (location = 2) out vec3 fragParticleSystemCenter;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 scale;
uniform vec3 particleSystemCenter;

void main()
{
    fragTexCoords = texCoords;
	fragPos = center;
	fragParticleSystemCenter = particleSystemCenter;

    mat4 model = mat4(1.f);
    model[3][0] = center.x;
    model[3][1] = center.y;
    model[3][2] = center.z;

    mat4 modelView = view * model;
    modelView[0][0] = 1;
	modelView[1][1] = 1;
	modelView[2][2] = 1;
	modelView[0][1] = 0;
	modelView[0][2] = 0;
	modelView[1][0] = 0;
	modelView[1][2] = 0;
	modelView[2][0] = 0;
	modelView[2][1] = 0;

    gl_Position = projection * modelView * scale * vec4(pos, 1.f);
}