#version 460

layout (location = 0) in vec3 pos;

layout(location = 0) out vec3 fragWorldPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(pos, 1.0);
    fragWorldPos = (worldPos).xyz;

    gl_Position = projection * view * worldPos;
}