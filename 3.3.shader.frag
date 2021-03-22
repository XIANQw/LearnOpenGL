#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 Texcoord;
out vec4 FragColor;

struct Material {
    sampler2D diffuseMap;
    sampler2D specularMap;
    vec3 Ka;
    vec3 Kd;
    vec3 Ks; // specular
    float shininess;
}; 

uniform Material material;
uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform bool myModel;


void main()
{
    vec3 normal = normalize(Normal);
    vec3 samplingDiffRes = vec3(texture(material.diffuseMap, Texcoord));
    vec3 samplingSpecRes = vec3(texture(material.specularMap, Texcoord));
    // ambient
    vec3 light = material.Ka * samplingDiffRes;
    // diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 lightDeca = lightColor / dot(lightPos - FragPos, lightPos - FragPos);
    light += material.Kd * lightDeca * max(dot(lightDir, normal), 0.0) * samplingDiffRes;
    // specular
    vec3 viewDir = normalize(cameraPos - FragPos);
    if(myModel){
        vec3 h = normalize(viewDir + lightDir);
        light += material.Ks * lightDeca * pow(max(dot(normal, h), 0.0), material.shininess) * samplingSpecRes;
    } else{
        vec3 reflectDir = reflect(-lightDir, normal);
        light += material.Ks * lightDeca * pow(max(dot(reflectDir, viewDir), 0.0), material.shininess) * samplingSpecRes;
    }
    FragColor = vec4(light, 1.0);
}