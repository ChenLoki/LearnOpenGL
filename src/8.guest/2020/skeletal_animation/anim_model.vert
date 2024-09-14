#version 330 core

layout(location = 0) in vec3 pos;// 模型坐标
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout(location = 5) in ivec4 boneIds; // 所有影响当前顶点的骨骼ID，索引
layout(location = 6) in vec4 weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoords;

// 骨骼动画的顶点着色器，核心任务就是要计算每个顶点的位置
void main()
{
    vec4 totalPosition = vec4(0.0f);

    // 影响这个顶点的骨骼最多有4根，计算这些骨骼对当前顶点的总体影响
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        if(boneIds[i] == -1) 
            continue;

        if(boneIds[i] >=MAX_BONES) 
        {
            totalPosition = vec4(pos,1.0f);
            break;
        }

        // finalBonesMatrices[boneIds[i]] 某根骨骼最终的变换矩阵，从根节点的变换叠加而来
        vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos,1.0f);

        // 考虑该骨骼对当前顶点的影响权重
        totalPosition += localPosition * weights[i];

        //
        vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
   }
	
    mat4 viewModel = view * model;
    gl_Position =  projection * viewModel * totalPosition;
	TexCoords = tex;
}
