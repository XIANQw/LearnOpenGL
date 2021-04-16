#version 330 core
layout (location = 0) in vec3 aPos;    // 位置变量的属性位置值为 0 
layout (location = 1) in vec2 aUV;   // 颜色变量的属性位置值为 1
//out vec3 ourColor;                     // 向片段着色器输出一个颜色
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(-aPos.x, aPos.y, aPos.z, 1.0); // add the xOffset to the x position of the vertex position
    TexCoord = aUV;
}