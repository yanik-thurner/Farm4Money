#version 430 core

in vec2 TexCoords;
layout(location = 0) out vec4 color;
layout(location = 1) out vec3 outNorm;

uniform sampler2D tex;
varying vec2 texCoord;

void main() {
	color = texture(tex, TexCoords);
	outNorm = vec3(0,0,0);
}