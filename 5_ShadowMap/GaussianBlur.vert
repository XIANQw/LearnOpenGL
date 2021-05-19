#version 330 core
layout (location = 0) in vec3 aPos;    // λ�ñ���������λ��ֵΪ 0 
layout(location = 1) in vec2 aTexcoord;

out vec2 Texcoord;
uniform mat4 model;

void main()
{
    Texcoord = aTexcoord;
    gl_Position = vec4(aPos, 1.0);
}