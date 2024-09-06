#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 T;
    vec3 B;
    vec3 N;
    
    vec3 PosColor;
    vec3 NormalColor;
    vec3 TanColor;
    vec3 BTanColor;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    
//    vec3 B = normalize(normalMatrix * aBitangent);
    // 在世界空间构建TBN矩阵
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    B = normalize(B);
    
    // 将世界空间的坐标传输到FS，在FS构建TBN矩阵，在FS阶段将法线转换到WS计算
//    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.T  = T;
    vs_out.B  = B;
    vs_out.N  = N;
        
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    vs_out.PosColor         = vec3(projection *view * model * vec4(aPos, 1.0))*0.5 + 0.5;
    vs_out.NormalColor      = normalize(normalMatrix * aNormal);
    vs_out.TanColor         = normalize(normalMatrix * T);
    vs_out.BTanColor        = normalize(normalMatrix * B);
}

