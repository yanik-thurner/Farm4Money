/*
*	Used the following tutorial to create this class
*	https://learnopengl.com/Model-Loading/Model
*	and got it to work with the existing classes of the ECG framework
*/

#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "GL\glew.h"

#include "Geometry.h"
#include "Shader.h"
#include "Material.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <memory>

class Model {

public:
	/*  Functions   */
	~Model();
	//Model() {}
	//Model(std::string const &path, std::shared_ptr<Material> mat, bool gamma = false) : gammaCorrection(gamma), material(mat) { loadModel(path); matlist = false; }
	//Model(std::string const &path, std::vector<std::shared_ptr<Material>> matvec, bool gamma = false) : gammaCorrection(gamma), material(matvec[0]), matvec(matvec) { loadModel(path); matlist = true; }
	Model(std::string const &path, std::shared_ptr<Shader> shader, std::shared_ptr<Shader> shadercolor, bool gamma = false) : gammaCorrection(gamma), shader(shader), shadercolor(shadercolor) { loadModel(path); } //matlist = false; }
	Model(std::vector<GeometryData> &meshes, std::vector<std::shared_ptr<Material>> matvec, std::shared_ptr<Shader> shader, std::shared_ptr<Shader> shadercolor, bool gamma = false);
	void draw(glm::vec3 cam_pos, float view_frustum);
	std::vector<GeometryData> getMeshes();
	std::vector<std::shared_ptr<Material>> Model::getMat();
	//std::vector<Geometry> getGeometry();


	void transform(glm::mat4 transformation);
	void setTransformMatrix(glm::mat4 transform);
	void setMaterial(std::shared_ptr<Material> material);
	void resetModelMatrix();
	void setMatVec(std::vector<std::shared_ptr<Material>> vec);
	void movehead(glm::mat4 trans);

	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> shadercolor;

private:
	/*  Model Data  */
	std::shared_ptr<Material> material;
	std::vector<Texture2> textures_loaded;
	std::vector<GeometryData> meshes;
	std::vector<Geometry*> geometry;
	std::string directory;
	bool gammaCorrection;
	bool matlist;
	std::vector<std::shared_ptr<Material>> matvec;

	

	glm::mat4 trans;

	/*  Functions   */
	void fillGeom();
	void loadModel(string const &path);
	void processNode(aiNode *node, const aiScene *scene);
	GeometryData processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture2> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
	unsigned int TextureFromFile(const char *path, const string &directory, bool gamma);
};


