#version 330 core
layout (location = 0) in vec3 aPos;    // λ�ñ���������λ��ֵΪ 0 
layout (location = 1) in vec2 aUV;   // ��ɫ����������λ��ֵΪ 1
//out vec3 ourColor;                     // ��Ƭ����ɫ�����һ����ɫ
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(-aPos.x, aPos.y, aPos.z, 1.0); // add the xOffset to the x position of the vertex position
    TexCoord = aUV;
}