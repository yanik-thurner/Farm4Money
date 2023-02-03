#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

out VertexData {
	vec3 position_world;
	vec3 normal_world;
	vec2 uv;
} vert;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;
uniform mat3 normalMatrix;
uniform float time;

const float pi = 3.14159;
const float numWaves = 1;
const float wavelength = 1;
const float frequency = 0.5;
const float amplitude = 0.1;
const float speed = 0.5;
const vec2 direction = vec2(1,1);

float height(float x, float z) {
    float frequency = 2*pi/wavelength;
    float phase = speed * frequency;
    float theta = dot(direction, vec2(x, z));
    return amplitude * sin(theta * frequency + time * phase);
}

float dWavedx(float x, float z) {
    float frequency = 2*pi/wavelength;
    float phase = speed * frequency;
    float theta = dot(direction, vec2(x, z));
    float A = amplitude * direction.x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedz(float x, float z) {
    float frequency = 2*pi/wavelength;
    float phase = speed * frequency;
    float theta = dot(direction, vec2(x, z));
    float A = amplitude * direction.y * frequency;
    return A * cos(theta * frequency + time * phase);
}

vec3 waveNormal(float x, float z) {
    float dx = dWavedx(x, z);
    float dz = dWavedz(x, z);

    vec3 n = vec3(-dx, 1.0, -dz);
    return normalize(n);
}

void main() {
	vert.uv = uv;

	vec3 position_ = position;
	position_.y = position_.y + height(position_.x, position_.z) * normal.y;

	vert.normal_world = normalMatrix * waveNormal(position_.x, position_.z);

	vec4 position_world_ = modelMatrix * vec4(position_, 1);
	vert.position_world = position_world_.xyz;
	gl_Position = viewProjMatrix * position_world_;
}