#version 330 core

uniform vec3 lightDir = normalize(vec3(-1.0, 0.0, 0.0));
uniform vec3 lightColor = vec3(1.0, 1.0, 1.0);
uniform vec3 objectColor = vec3(0.0, 0.5, 0.9);

in vec3 fragNormal;
out vec4 FragColor;

void main() {
    vec3 N = normalize(fragNormal);
    vec3 ambient = 0.3 * objectColor;
    vec3 diffuse = objectColor * lightColor * max(0.0, dot(-lightDir, N));
    FragColor = vec4(ambient + diffuse, 1.0);
}
