/*
* Copyright 2017 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#include "Material.h"

/* --------------------------------------------- */
// Base material
/* --------------------------------------------- */



Material::Material(std::shared_ptr<Shader> shader, glm::vec3 materialCoefficients, float specularCoefficient)
	: _shader(shader), _materialCoefficients(materialCoefficients), _alpha(specularCoefficient)
{
}

Material::~Material()
{
}

Shader* Material::getShader()
{
	return _shader.get();
}

void Material::setUniforms()
{
	_shader->setUniform("materialCoefficients", _materialCoefficients);
	_shader->setUniform("specularAlpha", _alpha);
}

/* --------------------------------------------- */
// Texture material
/* --------------------------------------------- */

TextureMaterial::TextureMaterial(std::shared_ptr<Shader> shader, glm::vec3 materialCoefficients, float specularCoefficient, std::shared_ptr<NewTexture> diffuseTexture)
	: Material(shader, materialCoefficients, specularCoefficient), _diffuseTexture(diffuseTexture)
{
}

TextureMaterial::~TextureMaterial()
{
}

void TextureMaterial::setUniforms()
{
	Material::setUniforms();

	_diffuseTexture->bind(0);
	_shader->setUniform("diffuseTexture", 0);
}

/* --------------------------------------------- */
// Color material
/* --------------------------------------------- */

ColorMaterial::ColorMaterial(std::shared_ptr<Shader> shader, glm::vec3 materialCoefficients, float specularCoefficient, glm::vec3 color)
	: Material(shader, materialCoefficients, specularCoefficient), color(color)
{
}

ColorMaterial::~ColorMaterial()
{
}

void ColorMaterial::setUniforms()
{
	Material::setUniforms();

	_shader->setUniform("in_color", color);
}