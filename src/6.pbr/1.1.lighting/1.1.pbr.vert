#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

out vec3 T;
out vec3 B;
out vec3 N;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;

// 顶点着色器的基本功能就是将顶点变换到proj空间
void main()
{
    TexCoords   = aTexCoords;
    WorldPos    = vec3(model * vec4(aPos, 1.0));
    Normal      = normalMatrix * aNormal;

    gl_Position =  projection * view * vec4(WorldPos, 1.0);// [-w,w]
}