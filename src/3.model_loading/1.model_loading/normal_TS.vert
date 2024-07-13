#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

// 输送到流水线下一个阶段的数据，都会做插值？？？
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

    // 这个矩阵是专门用来转变法线的
    mat3 normalMatrix = transpose(inverse(mat3(model)));

//    vec3 T = normalize(normalMatrix * aTangent);
//    vec3 T = normalize(normalMatrix * vec3(0.1,0.2,0.3));

    // aNormal本来是定义在切线空间的
    // 将法线转换到世界空间
    // 剩下的TB都是在世界空间定义的
    // TBN矩阵中，N是唯一重要且精确的，因为在片段着色器中的法线也是在切线空间下定义的
    // 片段着色器中的法线，表示的是法线对于TBN矩阵中，N轴方向的偏移
    
    /// TBN矩阵是通过模型本身自带的法线和切线构造出来的
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * vec3(0.1,0.1,0.1));

//    vec3 B = normalize(normalMatrix * aBitangent);
    // 在世界空间构建TBN矩阵
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    B = normalize(B);

    // 最后的计算在切线空间
    // 将世界空间的坐标转换到切线空间，并做插值
    mat3 TBN = transpose(mat3(T, B, N));
    vs_out.TangentLightPos = TBN * lightPos;// 每个片元的lightPos数据根据自身的TBN矩阵不同也会做插值
    vs_out.TangentViewPos  = TBN * viewPos; // 每个片元的viewPos 数据根据自身的TBN矩阵不同也会做插值
    vs_out.TangentFragPos  = TBN * vs_out.FragPos;// 每个顶点在世界空间的坐标转换到切线空间之后的坐标。后面转换为每个片元在切线空间的坐标

    gl_Position = projection * view * model * vec4(aPos, 1.0);

    // 调试代码
    vs_out.PosColor         = vec3(projection *view * model * vec4(aPos, 1.0))*0.5 + 0.5;
    vs_out.NormalColor      = normalize(normalMatrix * aNormal);
    vs_out.TanColor         = normalize(normalMatrix * T);
    vs_out.BTanColor        = normalize(normalMatrix * B);
}
