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


//#define N_POINTLIGHT 1
uniform PointLight light;
uniform SpotLight spotLight;

uniform vec3 cameraPos;

uniform int shadowmode;
uniform int filtermode;
uniform float lightBleedReduction;
uniform float minVariance;



// Shadow map related variables
#define NUM_SAMPLES 16
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10

#define LIGHTSIZE 0.06
#define NEAR_PLANE 10.0

#define EPS 1e-3
#define PI 3.141592653589793
#define PI2 6.283185307179586


highp float rand_1to1(highp float x ) { 
  // -1 -1
    return fract(sin(x)*10000.0);
}

highp float rand_2to1(vec2 uv ) { 
  // 0 - 1
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

float unpack(vec4 rgbaDepth) {
    const vec4 bitShift = vec4(1.0, 1.0/256.0, 1.0/(256.0*256.0), 1.0/(256.0*256.0*256.0));
    return dot(rgbaDepth, bitShift);
}

vec2 poissonDisk[NUM_SAMPLES];

void poissonDiskSamples( const in vec2 randomSeed) {

    float ANGLE_STEP = PI2 * float( NUM_RINGS ) / float( NUM_SAMPLES );
    float INV_NUM_SAMPLES = 1.0 / float( NUM_SAMPLES );

    float angle = rand_2to1( randomSeed ) * PI2;
    float radius = INV_NUM_SAMPLES;

    float radiusStep = radius;

    for( int i = 0; i < NUM_SAMPLES; i ++ ) {
        poissonDisk[i] = vec2( cos( angle ), sin( angle ) ) * pow( radius, 0.75 );
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
}

void uniformDiskSamples( const in vec2 randomSeed ) {

    float randNum = rand_2to1(randomSeed);
    float sampleX = rand_1to1( randNum ) ;
    float sampleY = rand_1to1( sampleX ) ;

    float angle = sampleX * PI2;
    float radius = sqrt(sampleY);

    for( int i = 0; i < NUM_SAMPLES; i ++ ) {
        poissonDisk[i] = vec2( radius * cos(angle) , radius * sin(angle)  );

        sampleX = rand_1to1( sampleY ) ;
        sampleY = rand_1to1( sampleX ) ;

        angle = sampleX * PI2;
        radius = sqrt(sampleY);
    }
}


float findBlocker( sampler2D shadowMap, vec2 uv, float zReceiver ) {
    float meanDepth = 0.0;
    float searchSize = max((zReceiver - 0.2) / zReceiver * LIGHTSIZE, 0.005);
    // poissonDisk
    poissonDiskSamples(uv);
    float bias = 0.01;
    int count = 0;
    for(int i=0; i<PCF_NUM_SAMPLES; i++){
        float zOcc = texture2D(shadowMap, uv + poissonDisk[i] * searchSize).r;
        if (zReceiver - bias > zOcc){
            meanDepth += zOcc;
            count += 1;
        } 
    }
    if(count == 0) return 0.0;
    return meanDepth / float(count);
}


float PCF(sampler2D shadowMap, vec3 coords, float filterSize, float bias) {
 
    float visibility = 0.0;
    if(coords.z > 1.0){
        return 0.0;
    }
    if(filtermode == 0){
        // poissonDisk
        poissonDiskSamples(coords.xy);
        for(int i=0; i<PCF_NUM_SAMPLES; i++){
            float zOcc = texture2D(shadowMap, coords.xy + filterSize * poissonDisk[i]).r;
            visibility += step(coords.z - bias, zOcc);
        }
        visibility /= float(PCF_NUM_SAMPLES);
    } 
    else if(filtermode == 1) {
        // uniformDisk
        uniformDiskSamples(coords.xy);
        for(int i=0; i<PCF_NUM_SAMPLES; i++){
            float zOcc = texture2D(shadowMap, coords.xy + poissonDisk[i] * filterSize).r;
            visibility += step(coords.z - bias, zOcc);
        }
        visibility /= float(PCF_NUM_SAMPLES);  
    } else {
        // 周围两圈采样
        for(int x=-1; x<=1; x++){
            for(int y=-1; y<=1; y++){
                float zOcc = texture2D(shadowMap, coords.xy + vec2(x + 0.5, y + 0.5) * filterSize).r;
                visibility += step(coords.z - bias, zOcc);
            }
        }
        visibility /= 9.0;
    }
  
    return visibility;
}


float PCSS(sampler2D shadowMap, vec3 coords, float bias){
    float zReceiver = coords.z;
    // STEP 1: avgblocker depth
    float avgblockerDepth = findBlocker(shadowMap, coords.xy, zReceiver);
    // Early out if no blocker found
    if(avgblockerDepth == 0.0) {
        return 1.0;
    }

    // STEP 2: penumbra size
    // penumbraRatio / Wlight = (Zrec - zOcc) / zOcc;
    float penumbraRatio = (zReceiver - avgblockerDepth) / avgblockerDepth * LIGHTSIZE;
    // STEP 3: filtering
    float visibility = PCF(shadowMap, coords, penumbraRatio, bias);
    return visibility;

}


float useShadowMap(sampler2D shadowMap, vec3 coords, float bias){
    float zOcc = texture2D(shadowMap, coords.xy).r;
    float visibility = step(coords.z - bias, zOcc);
    return visibility;
}


float linstep(float low, float high, float value){
    return clamp((value - low)/(high-low), 0.0, 1.0);
}

float VSM(sampler2D shadowMap, vec3 coords, float varianceMin, float lightBleedReduction){

    vec2 moments = texture2D(shadowMap, coords.xy).xy;
    float p = step(coords.z, moments.x);
    float variance = max(moments.y - moments.x * moments.x, varianceMin);
    
    float d = coords.z - moments.x;
    float pMax = linstep(lightBleedReduction, 1.0, variance / (variance + d*d));

    return min(max(pMax, p), 1.0);
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

float getClosestDepth(vec4 Coords)
{
    float closestDepth = texture(depthMap, Coords.xy).r;
    return closestDepth;
}

vec3 blinPhong(vec3 coords, vec3 samplingDiff, vec3 samplingSpec, float visibility){
    vec3 dir = normalize(light.Pos);
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(cameraPos - fs_in.FragPos);

    vec3 ambient = light.ambient * samplingDiff;
    vec3 diffuse = light.Color * max(dot(normal, dir), 0.0) * samplingDiff;
    vec3 reflectDir = reflect(-dir, normal);
    vec3 specular = light.specular * light.Color * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * samplingSpec;


    vec3 color = ambient + visibility * (diffuse + specular);  
    return color;
}

#define USE_PCF 0
#define USE_PCSS 1

void main()
{
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    // NDC 坐标转成 0-1 的UV坐标
    projCoords = projCoords * 0.5 + 0.5;


    vec3 samplingDiffRes = vec3(texture(material.diffuseMap1, fs_in.TexCoords));
    vec3 samplingSpecRes = vec3(0.2);
    if(material.useSpecularMap){
        samplingSpecRes = vec3(texture(material.specularMap1, fs_in.TexCoords));
    }
    vec3 viewDir = normalize(cameraPos - fs_in.FragPos);
    vec3 lightDir = normalize(light.Pos);
    vec3 normal = normalize(fs_in.Normal);
    float texSize = 4.0 / textureSize(depthMap, 0).x;
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.004);
    float visibility = 1.0;
    if (shadowmode == 0){
        visibility = VSM(depthMap, projCoords, minVariance, lightBleedReduction);
    } else if (shadowmode == 1) {
        visibility = PCF(depthMap, projCoords, texSize, bias);
    } else if (shadowmode == 2){
        visibility = useShadowMap(depthMap, projCoords, bias);
    } else if (shadowmode == 3){
        visibility = PCSS(depthMap, projCoords, bias);
    }
    vec3 color = blinPhong(projCoords, samplingDiffRes, samplingSpecRes, visibility);
    if(shadowmode == 4){
        float zOcc = texture2D(depthMap, projCoords.xy).r;
        FragColor = vec4(vec3(zOcc), 1.0);
    }else{
        FragColor = vec4(color, 1.0);    
    }
}

