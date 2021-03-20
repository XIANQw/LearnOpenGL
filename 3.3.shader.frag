#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec3 vertexLightColor;
out vec4 FragColor;

struct Material {
    vec3 Ka; // ambient
    vec3 Kd; // diffuse 
    vec3 Ks; // specular
    float shininess;
}; 

uniform Material material;
uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform bool myModel;
uniform bool vShaderLight;


void main()
{
    if(vShaderLight){
        FragColor = vec4(objColor * vertexLightColor, 1.0);
    } else{
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
        FragColor = vec4(objColor * light, 1.0);
    }

}