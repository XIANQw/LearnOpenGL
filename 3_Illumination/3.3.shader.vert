#version 330 core
layout (location = 0) in vec3 aPos;    // 位置变量的属性位置值为 0 
layout (location = 1) in vec3 aNormal;   // 法向量的属性位置值为 1

out vec3 Normal;
out vec3 FragPos;
out vec3 vertexLightColor;
// vShader variable
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

struct Material {
    vec3 Ka; // ambient
    vec3 Kd; // diffuse 
    vec3 Ks; // specular
    float shininess;
}; 
// fShader variable
uniform Material material;

uniform bool vShaderLight;
uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform bool myModel;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    Normal = aNormal;
    FragPos = vec3(model * vec4(aPos, 1.0));
    if(vShaderLight){
        // ambient
        vec3 light = material.Ka * lightColor;
        // diffuse
        vec3 lightDir = normalize(lightPos - FragPos);
        vec3 lightDeca = lightColor / dot(lightPos - FragPos, lightPos - FragPos);
        light += material.Kd * lightDeca  * max(dot(lightDir, normalize(Normal)), 0.0);
        // specular
        vec3 viewDir = normalize(cameraPos - FragPos);
        if(myModel){
            vec3 h = normalize(viewDir + lightDir);
            light += material.Ks * lightDeca * pow(max(dot(normalize(Normal), h), 0.0), material.shininess);
        } else{
            vec3 reflectDir = reflect(-lightDir, normalize(Normal));
            light += material.Ks * lightDeca * pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
        }
        vertexLightColor = light;
    }
}