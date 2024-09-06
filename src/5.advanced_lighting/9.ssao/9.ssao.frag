#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

// 切线空间+z半球采样
uniform vec3 samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
int kernelSize = 64;
float radius = 0.5;
float bias = 0.025;

// tile noise texture over screen based on screen dimensions divided by noise size
const vec2 noiseScale = vec2(800.0/4.0, 600.0/4.0);

uniform mat4 projection;

#define PI 3.141592653589793

highp float rand_1to1(highp float x) {
    // -1 -1
    return fract(sin(x) * 10000.0);
}

highp float rand_2to1(vec2 uv) {
    // 0 - 1
    const highp float a = 12.9898, b = 78.233, c = 43758.5453;
    highp float dt = dot(uv.xy, vec2(a, b)), sn = mod(dt, PI);
    return fract(sin(sn) * c);
}

void main()
{
    // get input for SSAO algorithm
    vec3 fragPos    = texture(gPosition, TexCoords).xyz;
    vec3 normal     = normalize(texture(gNormal, TexCoords).rgb);
//    vec3 randomVec  = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    // create TBN change-of-basis matrix: from tangent-space to view-space
    // 法线normal是模型空间里定义的法线，经过view*model矩阵转换到view空间
    // randomVec是一个XY平面内的随机方向，z值为0，这里与normal进行正交化计算，也可以认为是存在于view空间的
    // 1、normal是view空间的Z轴
    // 2、tangent是randomVec与法线正交化之后得到的垂直于normal的切线
    //


    //    randomVec = vec3(1.0,0,0);
    // randomVec取随机值的作用，就是偏转采样核心（64个向量）到不同的方向，获得更佳的随机性
    // 每个核心样本将会被用来偏移观察空间片段位置从而采样周围的几何体。
    // 我们在教程开始的时候看到，如果没有变化采样核心，我们将需要大量的样本来获得真实的结果。
    // 通过引入一个随机的转动到采样核心中，我们可以很大程度上减少这一数量。
    // 这个randomVec能不能通过取随机值的方式获得？
//    randomVec = vec3(rand_1to1(TexCoords.x*gl_FragCoord.y) , rand_1to1(TexCoords.y*gl_FragCoord.x),0);

    vec3 randomVec = vec3(1.0,1.0,1.0);
    if(gl_FragCoord.x > 800)
    {
        randomVec  = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    }
    else
    {
        // 直接随机采样一个方向，降噪
        // 不用后期进行模糊计算
        randomVec = vec3(rand_1to1(TexCoords.x*gl_FragCoord.y) , rand_1to1(TexCoords.y*gl_FragCoord.x),0);
    }


    vec3 tangent    = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent  = cross(normal, tangent);
    mat3 TBN        = mat3(tangent, bitangent, normal);
    
    // iterate over the sample kernel and calculate occlusion factor
    float occlusionColor = 0.0;
    float unOcclusionColor = 0.0;

    float depth = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        // samples是定义于切线空间的点，通过定义在view空间的TBN矩阵转换到view空间
        // from tangent to view-space

        // 这一行的samplePos是view空间的一条向量
        vec3 samplePos = TBN * samples[i];

        // fragPos为球心，向半球的samplePos方向延展，得到一个采样点
        samplePos = fragPos + samplePos * radius; // 这里samplePos则是一个点
        
        // project sample position (to sample texture) (to get position on screen/texture)
        // 这里的projection就是第一阶段做投影的projection
        vec4 offset = vec4(samplePos, 1.0);// view空间的一个采样点
        offset = projection * offset;       //[-w,w]    from view to clip-space 转换到裁剪空间
        offset.xyz /= offset.w;             //[-1,1]    perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5;//[0,1]     transform to range 0.0 - 1.0
        
        // get sample depth
        // 采样深度贴图，采样点UV的对应深度图中的最小深度
        // 我们使用offset向量的x和y分量采样线性深度纹理，从而获取样本位置从观察者视角的深度值(第一个不被遮蔽的可见片段)
        float closestDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample


        // range check & accumulate
        // 当检测一个靠近表面边缘的片段时，它将会考虑测试表面之下的表面的深度值；
        // 这些值将会(不正确地)影响遮蔽因子。我们可以通过引入一个范围检测从而解决这个问题
        // 我们引入一个范围测试，从而保证只有当被测深度值在取样半径内时，才会影响遮蔽因子。

        // fragPos.z 半球的球心Z值
        // sampleDepth 采样点与球心的深度差越大，说明这个点的影响越小
        // 这里使用abs(fragPos.z - samplePos.z)似乎更合适

        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
//        occlusion += (sampleDepth <= samplePos.z + bias ? 0.0 : 1.0) * rangeCheck;


//        if(samplePos.z+bias > sampleDepth)
//        {
//            // 遮挡
//            occlusionColor += 0.0*rangeCheck;
//        }
//        else
//        {
//            // 没有遮挡
//            occlusionColor += 1.0*rangeCheck;
//        }

        if(samplePos.z+bias > closestDepth)
        {
            // 遮挡
            occlusionColor += 1.0 * rangeCheck;
        }
        else
        {
            // 没有遮挡
            unOcclusionColor += 1.0 * rangeCheck;
        }


        // 我们接下来检查样本的当前深度值是否大于存储的深度值，如果是的，添加到最终的贡献因子上。
//        occlusion += (sampleDepth >= samplePos.z ? 1.0 : 0.0);

//        depth = (sampleDepth >= samplePos.z ? 1.0 : 0.0);
    }

    // occlusion / kernelSize 是所有没有遮挡的比例
//    occlusion = 1.0-(occlusion / kernelSize);

    float ao = occlusionColor/(occlusionColor + unOcclusionColor);
    
    FragColor = ao;;
}
