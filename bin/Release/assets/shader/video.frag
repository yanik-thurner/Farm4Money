#version 430 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D tex;
varying vec2 texCoord;

void main() {
	color = texture(tex, TexCoords);
}