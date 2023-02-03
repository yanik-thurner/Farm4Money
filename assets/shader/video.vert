#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out vec3 position_world;
out vec3 normal_world;
out vec2 TexCoords;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;
uniform mat3 normalMatrix;

void main() {
	normal_world = normalMatrix * normal;
	TexCoords = uv;
	vec4 position_world_ = modelMatrix * vec4(position, 1);
	position_world = position_world_.xyz;
	gl_Position = viewProjMatrix * position_world_;
}