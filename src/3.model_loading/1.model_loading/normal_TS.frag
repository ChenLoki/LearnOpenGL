#version 330 core
out vec4 FragColor;

in VS_OUT
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
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
//    FragColor = vec4(fs_in.PosColor.xyz, 1.0);
//    FragColor = vec4(fs_in.NormalColor.xyz, 1.0);
//    FragColor = vec4(fs_in.TanColor.xyz, 1.0);
//    FragColor = vec4(fs_in.BTanColor.xyz, 1.0);

//    return;

//      obtain normal from normal map in range [0,1]
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;

//     transform normal vector to range [-1,1]
    // 这里获得的normal法线是在切线空间描述的
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space

    vec3 color      = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 ambient    = color*0.1;

    // diffuse
    vec3 lightDir   = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff      = max(dot(lightDir, normal), 0.0);
    vec3 diffuse    = diff * color;

    // specular
    vec3 viewDir    = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec      = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.5) * spec;
    FragColor = vec4(ambient + diffuse + specular, 1.0);


}



