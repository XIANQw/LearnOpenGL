#version 330 core
layout (location = 0) in vec3 aPos;    // 位置变量的属性位置值为 0 
layout(location = 1) in vec2 aTexcoord;

out vec2 Texcoord;
uniform mat4 model;

void main()
{
    Texcoord = aTexcoord;
    gl_Position = vec4(aPos, 1.0);
}