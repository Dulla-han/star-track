#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

smooth out vec2 texCoord_v;
//view-space position
smooth out vec3 pos_v;
 //view-space normal
smooth out vec3 N;
out float fogDistance;

void main() {
    mat4 MV = view * model;
    vec4 viewPos = MV * vec4(position, 1.0);

    pos_v = viewPos.xyz;
    //surface normal
    N = normalize(mat3(transpose(inverse(MV))) * normal);

    fogDistance = length(pos_v);
    //transform position of vertex
    gl_Position = projection * viewPos;
    //send texture coordinations to the frag shader
    texCoord_v = texCoord;
}
