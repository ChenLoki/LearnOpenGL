#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT
{
    vec3 normal;
} vs_out;

uniform mat4 view;// 转到view空间是为了配合摄像机的运动（旋转和平移）
uniform mat4 model;

void main()
{
    // 计算view空间的转换矩阵
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));

    // 传递法线
    vs_out.normal = vec3(vec4(normalMatrix * aNormal, 0.0));

    // 转到到view空间计算
    gl_Position = view * model * vec4(aPos, 1.0);
}
