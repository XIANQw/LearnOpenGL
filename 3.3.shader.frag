#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 Texcoord;
out vec4 FragColor;

struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    float shininess;
}; 

struct DirLight{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 Dir;
};

struct PointLight{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float Kc;
    float Kl;
    float Kq;

    vec3 Pos;
    vec3 Color;
};

struct SpotLight{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    vec3 Pos;
    vec3 Dir;
    vec3 Color;

    float cutOff; // Inner cone
    float outerCutOff; // Outer cone
    
    // light's attenuation: Ft = 1 / (Kc + Kl*d + Kq*d*d)
    float Kc;
    float Kl;
    float Kq;
};

uniform Material material;
uniform PointLight pointLight;
uniform SpotLight spotLight;

uniform vec3 objColor;
uniform vec3 cameraPos;
uniform bool myModel;


vec3 computePointLight(){
    vec3 normal = normalize(Normal);
    vec3 samplingDiffRes = vec3(texture(material.diffuseMap, Texcoord));
    vec3 samplingSpecRes = vec3(texture(material.specularMap, Texcoord));
    // ambient
    vec3 light = pointLight.ambient * samplingDiffRes;
    
    // diffuse
    vec3 lightDir = normalize(pointLight.Pos - FragPos);
    float distance = length(pointLight.Pos - FragPos);
    float attenuation = 1.0 / (pointLight.Kc + pointLight.Kl * distance + 
                pointLight.Kq * (distance * distance));
    light += pointLight.diffuse * pointLight.Color * attenuation * max(dot(lightDir, normal), 0.0) * samplingDiffRes;
    // specular
    vec3 viewDir = normalize(cameraPos - FragPos);
    if(myModel){
        vec3 h = normalize(viewDir + lightDir);
        light += pointLight.specular * pointLight.Color * attenuation * pow(max(dot(normal, h), 0.0), material.shininess) * samplingSpecRes;
    } else{
        vec3 reflectDir = reflect(-lightDir, normal);
        light += pointLight.specular * pointLight.Color * attenuation * pow(max(dot(reflectDir, viewDir), 0.0), material.shininess) * samplingSpecRes;
    }
    return light;
}


vec3 computeSpotLight(){
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(spotLight.Pos - FragPos);
    float theta = dot(lightDir, normalize(-spotLight.Dir));
    float eps = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / eps, 0.0, 1.0); 
    
    vec3 samplingDiffRes = vec3(texture(material.diffuseMap, Texcoord));
    vec3 samplingSpecRes = vec3(texture(material.specularMap, Texcoord));
    // ambient
    vec3 ambient = spotLight.ambient * samplingDiffRes;
   
   // diffuse
    float distance = length(pointLight.Pos - FragPos);
    float attenuation = 1.0 / (spotLight.Kc + spotLight.Kl * distance + 
                pointLight.Kq * (distance * distance));
    vec3 diffuse = spotLight.diffuse * spotLight.Color * attenuation * max(dot(lightDir, normal), 0.0) * samplingDiffRes;
    
    // specular
    vec3 viewDir = normalize(cameraPos - FragPos);
    vec3 h = normalize(viewDir + lightDir);
    vec3 specular = spotLight.specular * spotLight.Color * attenuation * pow(max(dot(normal, h), 0.0), material.shininess) * samplingSpecRes;
    
    vec3 light = ambient + diffuse * intensity + specular * intensity;
    return light;
}


void main()
{
    vec3 light2 = computeSpotLight();
    FragColor = vec4(light2, 1.0);
}