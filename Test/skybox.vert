#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord; 

uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord_v;
const float PI = 3.14159265359;

void main() {
    vec3 dir = normalize(position);
    
    //got helped by ai, since I could not find what is wrong with sphere :(
    float u = atan(dir.z, dir.x) / (2.0 * PI) + 0.5;
    float v = asin(dir.y) / PI + 0.5;

    texCoord_v = vec2(u, 1.0f - v);

    mat4 viewNoTranslation = mat4(mat3(view));
    vec4 pos = projection * viewNoTranslation * vec4(position, 1.0);

    gl_Position = pos.xyww;
}