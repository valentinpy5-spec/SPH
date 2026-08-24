#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 instanceCenter;

uniform mat4 view;
uniform mat4 projection;
uniform float radius;

out vec3 fragNormal;

void main() {
    vec3 worldPos = instanceCenter + radius * vertexPos;
    gl_Position = projection * view * vec4(worldPos, 1.0);
    fragNormal = normalize(vertexNormal);
}
