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

uniform bool usePCSS;
uniform bool usePCF;
uniform bool useShadowmap;



// Shadow map related variables
#define NUM_SAMPLES 20
#define BLOCKER_SEARCH_NUM_SAMPLES NUM_SAMPLES
#define PCF_NUM_SAMPLES NUM_SAMPLES
#define NUM_RINGS 10

#define LIGHTSIZE 0.05
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
    float texelSize = 1.0 / 2048.0;
    // poissonDisk
    poissonDiskSamples(uv);
    float bias = 0.01;
    int count = 0;
    for(int i=0; i<PCF_NUM_SAMPLES; i++){
    float Zocc = texture2D(shadowMap, uv + poissonDisk[i] * texelSize).r;
    if (zReceiver - bias > Zocc){
        meanDepth += Zocc;
        count += 1;
    } 
    meanDepth += Zocc;
    }
    if(count == 0) return 0.0;
    return meanDepth / float(count);
}


float PCF(sampler2D shadowMap, vec3 coords, float filterSize) {
 
  float visibility = 0.0;
  float bias = 0.01;
  if(coords.z > 1.0){
    return 0.0;
  }
  // ��Χ��Ȧ����
  // for(int x=-2; x<=2; x++){
  //   for(int y=-2; y<=2; y++){
  //     float Zocc = texture2D(shadowMap, coords.xy + vec2(x, y) * filterSize).r;
  //     visibility += coords.z - bias > Zocc ? 0.0 : 1.0;
  //   }
  // }
  // visibility /= 25.0;

  // poissonDisk
  poissonDiskSamples(coords.xy);
  for(int i=0; i<PCF_NUM_SAMPLES; i++){
    float Zooc = texture2D(shadowMap, coords.xy + filterSize * poissonDisk[i]).r;
    visibility += coords.z - bias > Zooc ? 0.0 : 1.0;
  }

  // uniformDisk
  // uniformDiskSamples(coords.xy);
  // for(int i=0; i<PCF_NUM_SAMPLES; i++){
  //   float Zooc = texture2D(shadowMap, coords.xy + poissonDisk[i] * filterSize).r;
  //   visibility += coords.z - bias > Zooc ? 0.0 : 1.0;
  // }

  visibility /= float(PCF_NUM_SAMPLES);  
  return visibility;
}


float PCSS(sampler2D shadowMap, vec3 coords){
  float zReceiver = coords.z;
  float bias = 0.01;
  // STEP 1: avgblocker depth
  float avgblockerDepth = findBlocker(shadowMap, coords.xy, zReceiver);
  // Early out if no blocker found
  if(avgblockerDepth == 0.0) {
    return 1.0;
  }

  // STEP 2: penumbra size
  // penumbraRatio / Wlight = (Zrec - Zocc) / Zocc;
  float penumbraRatio = (zReceiver - avgblockerDepth) / avgblockerDepth * LIGHTSIZE;
  // STEP 3: filtering
  float visibility = PCF(shadowMap, coords, penumbraRatio);
  return visibility;

}


float useShadowMap(sampler2D shadowMap, vec3 shadowCoord){
  float Zocc = texture2D(shadowMap, shadowCoord.xy).r;
  float bias = 0.01;
  float visibility = shadowCoord.z - bias > Zocc ? 0.0: 1.0;
  return visibility;
}


float getCurrentDepth(vec4 fragPosLightSpace)
{
    // ִ��͸�ӳ���
    /*
    �������ڶ�����ɫ�����һ�����пռ䶥��λ�õ�gl_Positionʱ��
    OpenGL�Զ�����һ��͸�ӳ����������пռ�����ķ�Χ-w��wתΪ-1��1��
    ��Ҫ��x��y��zԪ�س���������wԪ����ʵ�֡�
    ���ڲ��пռ��FragPosLightSpace������ͨ��gl_Position����Ƭ����ɫ���
    ���Ǳ����Լ���͸�ӳ���
    */
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // NDC ����ת�� 0-1 ��UV����
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

vec3 blinPhong(vec3 coords, vec3 samplingDiff, vec3 samplingSpec, float visibility){
    vec3 dir = normalize(-dirLight.Dir);
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(cameraPos - fs_in.FragPos);

    vec3 ambient = dirLight.ambient * samplingDiff;
    vec3 diffuse = dirLight.Color * max(dot(normal, dir), 0.0) * samplingDiff;
    vec3 reflectDir = reflect(-dir, normal);
    vec3 specular = dirLight.specular * dirLight.Color * pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) * samplingSpec;


    vec3 color = ambient + visibility * (diffuse + specular);  
    return color;
}

#define USE_PCF 0
#define USE_PCSS 1

void main()
{
    vec3 projCoords = fs_in.FragPosLightSpace.xyz / fs_in.FragPosLightSpace.w;
    // NDC ����ת�� 0-1 ��UV����
    projCoords = projCoords * 0.5 + 0.5;


    vec3 samplingDiffRes = vec3(texture(material.diffuseMap1, fs_in.TexCoords));
    vec3 samplingSpecRes = vec3(0.2);
    if(material.useSpecularMap){
        samplingSpecRes = vec3(texture(material.specularMap1, fs_in.TexCoords));
    }
    vec3 viewDir = normalize(cameraPos - fs_in.FragPos);
    float texSize = 1.0 / textureSize(depthMap, 0).x;

    float visibility = 1.0;
    if (usePCF) {
        visibility = PCF(depthMap, projCoords, texSize);
    } else if (usePCSS){
        visibility = PCSS(depthMap, projCoords);
    } else {
        visibility = useShadowMap(depthMap, projCoords);
    }
    vec3 color = blinPhong(projCoords, samplingDiffRes, samplingSpecRes, visibility);

    FragColor = vec4(color, 1.0);    
}

