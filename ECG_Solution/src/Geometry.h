/*
* Copyright 2017 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once


#include <vector>
#include <memory>
#include <GL\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>

#include <glm\gtc\matrix_transform.hpp>
#include "Shader.h"
#include "Material.h"
#include <string>


struct GeometryData {
	std::vector<glm::vec3> positions;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
};

struct Vertex {
	// position
	glm::vec3 Position;
	// normal
	glm::vec3 Normal;
	// texCoords
	glm::vec2 TexCoords;
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;
};

struct Texture2 {
	unsigned int id;
	string type;
	string path;
};

class Geometry
{
protected:

	GLuint _vao;
	GLuint _vboPositions;
	GLuint _vboNormals;
	GLuint _vboUVs;
	GLuint _vboIndices;
	unsigned int _elements;
 
	std::shared_ptr<Material> _material;
	std::vector<Texture2> textures;

	
	
	bool _isEmpty;
	std::vector<std::shared_ptr<Geometry>> _children;

public:
	//Geometry(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	Geometry(glm::mat4 modelMatrix, GeometryData& data, std::shared_ptr<Material> material);
	Geometry(glm::mat4 modelMatrix = glm::mat4(1.0f));

	~Geometry();

	void draw(glm::mat4 matrix = glm::mat4(1.0f));

	glm::mat4 _transformMatrix;
	glm::mat4 _modelMatrix;

	void transform(glm::mat4 transformation);
	void setTransformMatrix(glm::mat4 transformMatrix);
	void resetModelMatrix();

	Geometry* addChild(std::shared_ptr<Geometry> child);


	static GeometryData createMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture2> textures);
	static GeometryData createCubeGeometry(float width, float height, float depth);
	static GeometryData createCylinderGeometry(unsigned int segments, float height, float radius);
	static GeometryData createSphereGeometry(unsigned int longitudeSegments, unsigned int latitudeSegments, float radius);
};