#version 330 core

in vec2 texCoord_v;

uniform sampler2D skyboxSampler;

out vec4 FragColor;

void main() {
    FragColor = texture(skyboxSampler, texCoord_v);
}
