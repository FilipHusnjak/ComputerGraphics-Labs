#version 460

layout(location = 0) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

const vec3 baseColor = vec3(0.18,0.27,0.47);

void main()
{
    float red = -0.00022 * (abs(fragWorldPos.y) - 2800) + 0.18;
    float green = -0.00025 * (abs(fragWorldPos.y) - 2800) + 0.27;
    float blue = -0.00019 * (abs(fragWorldPos.y) - 2800) + 0.5;
    outColor = vec4(red, green, blue, 1);
}
