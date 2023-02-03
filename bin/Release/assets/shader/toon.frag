#version 430 core

const int MAX_PLIGHTS = 10; 
const int LEVELS = 4;

in VertexData {
	vec3 position_world;
	vec3 normal_world;
	vec2 uv;
} vert;

layout(location = 0) out vec4 color;
layout(location = 1) out vec3 outNorm;

uniform vec3 camera_world;
uniform vec3 materialCoefficients; // x = ambient, y = diffuse, z = specular 
uniform float specularAlpha;
uniform sampler2D diffuseTexture;

uniform struct DirectionalLight {
	vec3 color;
	vec3 direction;
	int amb;
} dirL;

uniform struct PointLight {
	vec3 color;
	vec3 position;
	vec3 attenuation;
} pointL[MAX_PLIGHTS];

float luma(vec3 color) {
  return dot(color, vec3(0.299, 0.587, 0.114));
}

void main() {	
	vec3 n = normalize(vert.normal_world);
	vec3 v = normalize(camera_world - vert.position_world);
	
	/*Directional light*/ //https://de.wikipedia.org/wiki/Phong-Beleuchtungsmodell
	//Iamb = Ia * Ka
	float Iamb = luma(dirL.color) * materialCoefficients.x;
	//Idiff = Iin * Kdiff * dot(normal,pointLrichtung)
	//float Idiff = luma(dirL.color) * materialCoefficients.y * max(0,dot(normalize(vert.normal_world),normalize(-dirL.direction)));
	//Ispec = Iin * Kspec * dot(reflextionR,KameraR)^rauigkeit
	//float Ispec = luma(pointL.color) * materialCoefficients.z * pow(max(0,dot(normalize(reflect(-normalize(-dirL.direction),n)),v)),specularAlpha);
	
	float Ispec = 0;
	float Idiff = 0;

	/*Point light*/
	for(int i = 0; i < 5; i++) {
		float d = length(pointL[i].position - vert.position_world);
		float att = 1.0f / (pointL[i].attenuation.x + d * pointL[i].attenuation.y + d * d * pointL[i].attenuation.z);

		//Idiff = Iin * Kdiff * dot(normal,pointLrichtung)
		Idiff += luma(pointL[i].color) * materialCoefficients.y * max(0,dot(normalize(vert.normal_world),normalize(pointL[i].position - vert.position_world))) * att;
		//Ispec = Iin * Kspec * dot(reflextionR,KameraR)^rauigkeit
		Ispec += luma(pointL[i].color) * materialCoefficients.z * pow(max(0,dot(normalize(reflect(-normalize(pointL[i].position - vert.position_world),n)),v)),specularAlpha) * att;
	}


	vec3 texColor = texture(diffuseTexture, vert.uv).rgb;

	color = vec4(texColor * (materialCoefficients.x+dirL.amb), 1);

	float intensity = floor((Iamb + Idiff + Ispec) * LEVELS) / LEVELS;
	color = color * intensity;
	//color = texture(diffuseTexture, vert.uv);
	//color = vec4(vert.normal_world, 1.0);
	outNorm = vert.normal_world;
}

