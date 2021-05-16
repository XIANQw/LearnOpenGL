#version 330 core

void main()
{          

    float depth = gl_FragCoord.z;
    float dx = dFdx(depth);
    float dy = dFdy(depth);
    float moments2 = depth * depth + 0.25 * (dx * dx + dy * dy);

    gl_FragColor = vec4(depth, moments2, 0.0, 0.0);
}