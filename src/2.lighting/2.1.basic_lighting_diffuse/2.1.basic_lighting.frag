#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // 计算Li * cos_theta
    vec3 norm       = normalize(Normal);
    vec3 lightDir   = normalize(lightPos - FragPos);
    float diff      = max(dot(norm, lightDir), 0.0);
    vec3 diffuse    = diff * lightColor;// lightColor 是radiance，向四面八方反射出的能量都是一样的
            
    // 计算f_r * Li * cos_theta
//    float albedo = 0.8;
//    float f_r = albedo / 3.1415926;
//    diffuse *= f_r;

    // 为什么乘f_r之后颜色会变暗?不乘f_r首先是错误的计算，albedo/PI是BRDF正常的能量损失

    vec3 result = (ambient + diffuse) * objectColor;


    FragColor = vec4(result, 1.0);
} 