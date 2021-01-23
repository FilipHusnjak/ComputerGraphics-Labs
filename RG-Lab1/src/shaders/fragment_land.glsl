#version 460 core

layout (location = 0) in vec2 fragTexCoord;
layout (location = 2) in vec3 fragWorldPos;

out vec4 outColor;

uniform vec3 cameraPos;

uniform sampler2D tex;
uniform sampler2D normalMap;

vec3 computeDiffuse(vec3 diffuse, vec3 ambient, vec3 lightDir, vec3 normal)
{
    float dotNL = max(dot(normal, lightDir), 0.0);
    vec3 c = diffuse * dotNL;
    return ambient + c;
}

const float PI = 3.14159265;

vec3 computeSpecular(vec3 spec, vec3 viewDir, vec3 lightDir, vec3 normal)
{
    const float kShininess = 4.0;
    if (dot(lightDir, normal) < 0) return vec3(0);
    vec3 r = normalize(2 * dot(lightDir, normal) * normal - lightDir);
    float specular = pow(max(dot(r, viewDir), 0.0), kShininess);
    return vec3(spec * specular);
}

void main()
{
    vec3 lightDir = {1, -1, 0};
    lightDir = normalize(lightDir);
    float lightIntensity = 0.6;

    vec4 normalMapColor = texture(normalMap, fragTexCoord);
    vec3 normal = normalize(vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 3, normalMapColor.g * 2.0 - 1.0));

    vec3 diffuse = {0.8, 0.8, 0.8};
    vec3 ambient = {0.4, 0.4, 0.4};
    vec3 diffuseColor = computeDiffuse(diffuse, ambient, lightDir, normal);
    diffuseColor *= texture(tex, fragTexCoord).xyz;

    vec3 viewDir = normalize(cameraPos - fragWorldPos);
    vec3 specular = {0.05, 0.05, 0.05};
    vec3 specularColor = computeSpecular(specular, viewDir, lightDir, normal);

    float gamma = 1. / 2.2;
    outColor = pow(vec4(lightIntensity * (diffuseColor + specularColor), 1.f), vec4(gamma));
}