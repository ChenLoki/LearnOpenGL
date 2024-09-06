#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;

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


//    mat3 normalMatrix = transpose(inverse(mat3(model)));
    N = normalize(normalMatrix * aNormal);
    T = normalize(normalMatrix * vec3(0.3,0.7,0.9));
    T = normalize(T - dot(T, N) * N);
    B = cross(N, T);
    B = normalize(B);

    gl_Position =  projection * view * vec4(WorldPos, 1.0);// [-w,w]
}