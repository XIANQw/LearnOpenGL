#version 330 core
layout (location = 0) in vec3 aPos;    // 位置变量的属性位置值为 0 
layout (location = 1) in vec2 aUV;   // 颜色变量的属性位置值为 1

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0); // add the xOffset to the x position of the vertex position
}