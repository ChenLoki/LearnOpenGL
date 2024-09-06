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
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    
    vec3 PosColor;
    vec3 NormalColor;
    vec3 TanColor;
    vec3 BTanColor;
    vec3 YuanshiNormal;
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
//    mat3 normalMatrix = mat3(model);
//    mat3 normalMatrix = inverse(transpose((mat3(model))));

    /// 这里的aNormal是定义在模型坐标系或者世界坐标系的
    // 是用来构建TBN矩阵的，aNormal的方向永远是这个TBN矩阵的Z轴方向
    // 片段着色器中采样得到的normal表示的是对这个aNormal的方向偏移
    // aNormal是基准，normal是相对于基准的偏移
    // 两者的方向是并不是一个概念
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * vec3(0.3,0.7,0.9));
//    vec3 T = normalize(normalMatrix * aTangent);
//    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    B = normalize(B);
    
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos  = TBN * viewPos;
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
        
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    
    vs_out.PosColor         = vec3(projection *view * model * vec4(aPos, 1.0))*0.5 + 0.5;
    vs_out.NormalColor      = normalize(normalMatrix * aNormal);
    vs_out.TanColor         = normalize(normalMatrix * T);
    vs_out.BTanColor        = normalize(normalMatrix * B);
    vs_out.YuanshiNormal    = aNormal*0.5+0.5;
}
