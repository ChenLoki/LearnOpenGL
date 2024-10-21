#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;
uniform float roughness;

const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

// ----------------------------------------------------------------------------
// xi就是随机变量ξ，在[0,1]上分布的随机变量，这里是二维的

//这个函数返回一个向量，该向量大体围绕着预估的微表面的半向量
// 不同的粗糙度会决定不同的法线分布函数D(m)
// D(m)本身是一个概率密度函数，因此可以反推出与微表面法线方向m夹角为θ的方向
// 这个反推得到的方向就是重要性采样的微表面的分布方向
// 再根据这个微表面方向，得到入射光线的方向，也就是环境贴图的采样方向
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness*roughness;

    // 这样取到的随机变量的方向是符合NDF法线分布函数的
    // F*D*G中的D(m)实际上是一个关于微表面法线m的分布函数，其积分 ∫D(m)(n*m)dw 结果也是1
    // θ是表面微法线与表面法线的夹角，微法线的方向，与半程向量h的方向是一致的


    ///---------------------------- 这里算出来的是微表面的法线m的角度----------------------------

    // φ方向是均匀分布的
	float phi = 2.0 * PI * Xi.x;

    // 按照下面的公式得到的θ，是符合 ∫D(m)(n*m)dw 概率密度分布的θ
    // 不同粗糙度的D(m)是不一样的，这样就能得到不同粗糙度下的θ随机取值了
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));// θ方向需要取低差异序列随机数
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);

    ///---------------------------- 这里算出来的是微表面的法线m的角度----------------------------
	
	// from spherical coordinates to cartesian coordinates - halfway vector
    // 得到符合D(m)分布的随机取值(θ,φ)之后，根据(θ,φ)就能计算笛卡尔坐标
    // 知道角度(θ,φ)，就一定能够得到笛卡尔坐标系的方向向量(x,y,z)
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	// from tangent-space H vector to world-space sample vector
    // 从切线空间转换到世界空间
	vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);
	
	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}
// ----------------------------------------------------------------------------



 //预过滤这个shader计算的是，不同粗糙度下的环境cubemap
void main()
{
    vec3 N = normalize(WorldPos);

    // make the simplifying assumption that V equals R equals the normal
    // 由于我们在卷积环境贴图时事先不知道视角方向，
    // 因此 Epic Games 假设视角方向——也就是镜面反射方向——总是等于输出采样方向ωo
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    // 之前diffuse的irradiance是朝半球面所有入射光方向做卷积
    // 现在是根据材质的粗糙度，重要性的采样一些入射光方向

    // 预滤波器卷积着色器
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        // generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
        // Xi.x是在[0,1]上均分分布的
        // Xi.y是一个[0,1]上的低差异随机序列
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);

        // ImportanceSampleGGX函数的返回值就是微表面的法线方向，也就是半程向量的方向
        // 采样得到一个符合D(m)分布的随机半程向量
        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);

        // 根据半程向量H方向，视角V方向计算入射光方向L
        // 这里才真正得到采样的入射光方向
        // 做预计算时，视角方向V这里简化为了宏观平面的法线N的方向
        vec3 L  = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if(NdotL > 0.0)
        {
            // sample from the environment's mip level based on roughness/pdf
            float D   = DistributionGGX(N, H, roughness);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);




            // 我们可以在预过滤卷积时，不直接采样环境贴图，
            // 而是基于积分的 PDF 和 粗糙度，采样环境贴图的 mipmap ，以减少伪像
            // 如果直接采样environmentMap，会有很多高频信号

            //光线L的pdf_L 与半向量 H 是存在函数关系的
            // pdf_L = pdf_H/(4*L*H)
            // pdf_H = D * cos(m) = D * NdotH
            float pdf_L = D * NdotH / (4.0 * HdotV) + 0.0001;// 这里是light的pdf

            float resolution = 512.0; // resolution of source cubemap (per face) 一个面是像素数是512*512

            // cubemap纹理6个面是像素数是6.0 * resolution * resolution
            // 每个纹素对应多少弧度
            float saTexel  = 4.0 * PI / (6.0 * resolution * resolution); // 2、这里是整个  球面的立体角/6个面总的像素数 ？？？？
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf_L + 0.0001);

            // 3、三线性插值？？？
            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

            prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
}


//void main()
//{
//    vec3 N = normalize(WorldPos);
//    vec3 R = N;
//    vec3 V = R;
//
//    const uint SAMPLE_COUNT = 1024u;
//    float totalWeight = 0.0;
//    vec3 prefilteredColor = vec3(0.0);
//    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
//    {
//        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
//        vec3 H  = ImportanceSampleGGX(Xi, N, roughness);
//        vec3 L  = normalize(2.0 * dot(V, H) * H - V);
//
//        float NdotL = max(dot(N, L), 0.0);
//        if(NdotL > 0.0)
//        {
////            prefilteredColor += textureLod(environmentMap, L , 5.5).rgb * NdotL;
//            prefilteredColor += texture(environmentMap, L).rgb * NdotL;
//
//// 用 prefilteredColor 除以采样权重总和，其中对最终结果影响较小（NdotL 较小）的采样最终权重也较小
//// 还是加权平均的思想
//            totalWeight      += NdotL;
//        }
//    }
//    prefilteredColor = prefilteredColor / totalWeight;
//
//    FragColor = vec4(prefilteredColor, 1.0);
//}