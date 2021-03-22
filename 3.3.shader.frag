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

    vec3 direction;
};

struct PointLight{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float Kc;
    float Kl;
    float Kq;

    vec3 lightPos;
    vec3 lightColor;
};

uniform Material material;
uniform PointLight pointLight;

uniform vec3 objColor;
uniform vec3 cameraPos;
uniform bool myModel;


void main()
{
    vec3 normal = normalize(Normal);
    vec3 samplingDiffRes = vec3(texture(material.diffuseMap, Texcoord));
    vec3 samplingSpecRes = vec3(texture(material.specularMap, Texcoord));
    // ambient
    vec3 light = pointLight.ambient * samplingDiffRes;
    // diffuse
    vec3 lightDir = normalize(pointLight.lightPos - FragPos);
    vec3 lightDeca = pointLight.lightColor / dot(pointLight.lightPos - FragPos, pointLight.lightPos - FragPos);
    light += pointLight.diffuse * lightDeca * max(dot(lightDir, normal), 0.0) * samplingDiffRes;
    // specular
    vec3 viewDir = normalize(cameraPos - FragPos);
    if(myModel){
        vec3 h = normalize(viewDir + lightDir);
        light += pointLight.specular * lightDeca * pow(max(dot(normal, h), 0.0), material.shininess) * samplingSpecRes;
    } else{
        vec3 reflectDir = reflect(-lightDir, normal);
        light += pointLight.specular * lightDeca * pow(max(dot(reflectDir, viewDir), 0.0), material.shininess) * samplingSpecRes;
    }
    FragColor = vec4(light, 1.0);
}