
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 outNorm;


uniform sampler2D screenTexture;

out vec2 TexCoords;
out vec3 texNormal;

void main()
{
    gl_Position = vec4(position, 0.0f, 1.0f); 
    TexCoords = texcoord;
    texNormal = outNorm;
}  