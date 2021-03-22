#version 330 core
layout (location = 0) in vec3 aPos;    // λ�ñ���������λ��ֵΪ 0 
layout (location = 1) in vec3 aNormal;   // ������������λ��ֵΪ 1
layout(location = 2) in vec2 aTexcoord;

out vec3 Normal;
out vec3 FragPos;
out vec2 Texcoord;
// vShader variable
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    // �����ߴ�ģ�Ϳռ�ת��������ռ�
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    Texcoord = aTexcoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}