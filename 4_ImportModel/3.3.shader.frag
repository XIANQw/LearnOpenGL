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
    vec3 Color;
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
uniform DirLight dirLight;
#define N_POINTLIGHT 1
uniform PointLight pointLights[N_POINTLIGHT];
uniform SpotLight spotLight;

uniform vec3 objColor;
uniform vec3 cameraPos;
uniform bool useSpotLight;


vec3 computeDirLight(DirLight dirLight, vec3 viewDir, vec3 samplingDiff, vec3 samplingSpec){
    vec3 dir = normalize(-dirLight.Dir);
    vec3 ambient = dirLight.ambient * samplingDiff;
    vec3 diffuse = dirLight.diffuse * dirLight.Color * max(dot(Normal, dir), 0.0) * samplingDiff;
    vec3 reflectDir = reflect(-dir, Normal);
    vec3 specular = dirLight.specular * dirLight.Color * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * samplingSpec;

    return ambient + diffuse + specular;
}

vec3 computePointLight(PointLight pointLight, vec3 viewDir, vec3 samplingDiff, vec3 samplingSpec){
    vec3 normal = normalize(Normal);

    // ambient
    vec3 ambient = pointLight.ambient * samplingDiff;
    
    // diffuse
    vec3 lightDir = normalize(pointLight.Pos - FragPos);
    vec3 diffuse = pointLight.diffuse * pointLight.Color * max(dot(lightDir, normal), 0.0) * samplingDiff;
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = pointLight.specular * pointLight.Color * pow(max(dot(reflectDir, viewDir), 0.0), material.shininess) * samplingSpec;
   
    float distance = length(pointLight.Pos - FragPos);
    float attenuation = 1.0 / (pointLight.Kc + pointLight.Kl * distance + 
                pointLight.Kq * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}


vec3 computeSpotLight(SpotLight spotLight, vec3 viewDir, vec3 samplingDiff, vec3 samplingSpec){
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(spotLight.Pos - FragPos);
    float theta = dot(lightDir, normalize(-spotLight.Dir));
    float eps = spotLight.cutOff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / eps, 0.0, 1.0); 
    
    // ambient
    vec3 ambient = spotLight.ambient * samplingDiff;
   
   // diffuse
    vec3 diffuse = spotLight.diffuse * spotLight.Color * max(dot(lightDir, normal), 0.0) * samplingDiff;
    
    // specular
    vec3 h = normalize(viewDir + lightDir);
    vec3 specular = spotLight.specular * spotLight.Color * pow(max(dot(normal, h), 0.0), material.shininess) * samplingSpec;
    
    float distance = length(spotLight.Pos - FragPos);
    float attenuation = 1.0 / (spotLight.Kc + spotLight.Kl * distance + 
                spotLight.Kq * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}


void main()
{
    vec3 samplingDiffRes = vec3(texture(material.diffuseMap, Texcoord));
    vec3 samplingSpecRes = vec3(texture(material.specularMap, Texcoord));
    vec3 viewDir = normalize(cameraPos - FragPos);

    vec3 result = computeDirLight(dirLight, viewDir, samplingDiffRes, samplingSpecRes);
    for(int i=0; i<N_POINTLIGHT; i++){
        result+=computePointLight(pointLights[i], viewDir, samplingDiffRes, samplingSpecRes);
    }
    if(useSpotLight){
        result += computeSpotLight(spotLight, viewDir, samplingDiffRes, samplingSpecRes);
    }
    
    FragColor = vec4(result, 1.0);
}