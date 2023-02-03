#version 430 core

layout(location = 0) in vec3 quad;
layout(location = 1) in vec4 pos; //w is size

out vec2 uv;

uniform vec3 CameraRight;
uniform vec3 CameraUp;
uniform mat4 viewProjMatrix;

void main()
{
	float particleSize = pos.w;
	vec3 particleCenter = pos.xyz;
	
	vec3 vertexPosition_worldspace = 
		particleCenter + CameraRight * quad.x * particleSize + CameraUp * quad.y * particleSize;

	gl_Position = viewProjMatrix * vec4(vertexPosition_worldspace, 1.0f);
	//center texture
	uv = quad.xy + vec2(0.5, 0.5);
}