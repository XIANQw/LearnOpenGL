#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

out vec4 FragColor;

struct Material {
    sampler2D diffuseMap1;
    sampler2D specularMap1;
    bool useSpecularMap;
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

uniform sampler2D depthMap;
uniform Material material;

uniform DirLight dirLight;
#define N_POINTLIGHT 1
uniform PointLight pointLights[N_POINTLIGHT];
uniform SpotLight spotLight;

uniform vec3 cameraPos;

uniform bool compare;


float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // 执行透视除法
    /*
    当我们在顶点着色器输出一个裁切空间顶点位置到gl_Position时，
    OpenGL自动进行一个透视除法，将裁切空间坐标的范围-w到w转为-1到1，
    这要将x、y、z元素除以向量的w元素来实现。
    由于裁切空间的FragPosLightSpace并不会通过gl_Position传到片段着色器里，
    我们必须自己做透视除法
    */
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // NDC 坐标转成 0-1 的UV坐标
    projCoords = projCoords * 0.5 + 0.5;
    
    float currentDepth = projCoords.z;
    if(currentDepth > 1.0){
        return 0.0;
    }
    float shadow = 0.0;
    float bias = 0.005;
    if(compare){
        //PCF
        float bias = max(0.05 * (1.0 - dot(lightDir, normal)) / 2.0, 0.005);
        vec2 texelSize = 1.0 / textureSize(depthMap, 0);
        for(int x = -1; x <= 1; x++){
            for(int y = -1; y <= 1; y++){
                float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 9.0;
    } else{
        float closestDepth = texture(depthMap, projCoords.xy).r;
        
        shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0; 
    }
    return shadow;
}

float getCurrentDepth(vec4 fragPosLightSpace)
{
    // 执行透视除法
    /*
    当我们在顶点着色器输出一个裁切空间顶点位置到gl_Position时，
    OpenGL自动进行一个透视除法，将裁切空间坐标的范围-w到w转为-1到1，
    这要将x、y、z元素除以向量的w元素来实现。
    由于裁切空间的FragPosLightSpace并不会通过gl_Position传到片段着色器里，
    我们必须自己做透视除法
    */
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // NDC 坐标转成 0-1 的UV坐标
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    return currentDepth;
}

float getClosestDepth(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(depthMap, projCoords.xy).r;
    return closestDepth;
}

vec3 computeDirLight(vec3 samplingDiff, vec3 samplingSpec){
    vec3 dir = normalize(-dirLight.Dir);
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(cameraPos - fs_in.FragPos);

    vec3 ambient = dirLight.ambient * samplingDiff;
    vec3 diffuse = dirLight.Color * max(dot(normal, dir), 0.0) * samplingDiff;
    vec3 reflectDir = reflect(-dir, normal);
    vec3 specular = dirLight.specular * dirLight.Color * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * samplingSpec;

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, normal, dir);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));  
    return lighting;
}

vec3 computePointLight(PointLight pointLight, vec3 viewDir, vec3 samplingDiff, vec3 samplingSpec){
    vec3 normal = normalize(fs_in.Normal);

    // ambient
    vec3 ambient = pointLight.ambient * samplingDiff;
    
    // diffuse
    vec3 lightDir = normalize(pointLight.Pos - fs_in.FragPos);
    vec3 diffuse = pointLight.diffuse * pointLight.Color * max(dot(lightDir, normal), 0.0) * samplingDiff;
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 specular = pointLight.specular * pointLight.Color * pow(max(dot(reflectDir, viewDir), 0.0), material.shininess) * samplingSpec;
   
    float distance = length(pointLight.Pos - fs_in.FragPos);
    float attenuation = 1.0 / (pointLight.Kc + pointLight.Kl * distance + 
                pointLight.Kq * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}


vec3 computeSpotLight(SpotLight spotLight, vec3 viewDir, vec3 samplingDiff, vec3 samplingSpec){
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(spotLight.Pos - fs_in.FragPos);
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
    
    float distance = length(spotLight.Pos - fs_in.FragPos);
    float attenuation = 1.0 / (spotLight.Kc + spotLight.Kl * distance + 
                spotLight.Kq * (distance * distance));
    ambient *= attenuation;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ambient + diffuse + specular;
}


void main()
{
    vec3 samplingDiffRes = vec3(texture(material.diffuseMap1, fs_in.TexCoords));
    vec3 samplingSpecRes = vec3(0.2);
    if(material.useSpecularMap){
        samplingSpecRes = vec3(texture(material.specularMap1, fs_in.TexCoords));
    }
    vec3 viewDir = normalize(cameraPos - fs_in.FragPos);
    FragColor = vec4(computeDirLight(samplingDiffRes, samplingSpecRes), 1.0);    
}

