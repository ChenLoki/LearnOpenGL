#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm       = normalize(Normal);
    vec3 lightDir   = normalize(lightPos - FragPos);
    float diff      = max(dot(norm, lightDir), 0.0);
    vec3 diffuse    = diff * lightColor;

    // 计算f_r * Li * cos_theta
//    float albedo = 0.8;
//    float f_r = albedo / 3.1415926;
//    diffuse *= f_r;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir    = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 halfDir    = normalize(lightDir + viewDir);
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), 64);
//    float spec      = pow(max(dot(halfDir, norm), 0.0), 64);
    vec3 specular   = specularStrength * spec * lightColor;
        
    vec3 result = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(result, 1.0);
} 