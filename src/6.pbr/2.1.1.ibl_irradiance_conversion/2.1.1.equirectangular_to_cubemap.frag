#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

// 采样的是读取的等距柱状投影图
uniform sampler2D equirectangularMap;

// 柱状纹理转换为cubemap
const vec2 invAtan = vec2(0.1591, 0.3183);

// 将世界坐标转换为柱状图坐标
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(WorldPos));
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}
