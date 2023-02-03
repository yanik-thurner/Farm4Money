#pragma once

#include "Model.h"
#include "Shader.h"
#include <vector>

struct ModelMetaData {
	std::string file_path;
	std::shared_ptr<Shader> s1;
	std::shared_ptr<Shader> s2;
	std::vector<GeometryData> meshes;
	std::vector<std::shared_ptr<Material>> matvec;

	ModelMetaData(std::string file_path, std::vector<GeometryData> meshes, std::vector<std::shared_ptr<Material>> matvec, std::shared_ptr<Shader> s1, std::shared_ptr<Shader> s2)
		: file_path(file_path), meshes(meshes), matvec(matvec), s1(s1), s2(s2) {};
};

class ModelManager
{
private:
	ModelManager();
	
	static ModelManager *m;
	std::vector<std::vector<ModelMetaData>> model_vectors;

public:
	~ModelManager();
	static ModelManager *instance();

	enum ModelID {
		CUBE = 0,
		FIELD = 1,
		LANTERN = 2,
		PLAYER = 3,
		SELLBOX = 4,
		CORNPLANT = 5,
		CARROTPLANT = 6,
		MELONPLANT = 7,
		TOMATOPLANT = 8,
		SHOP = 9,
		SALESIGN = 10,
		WATER = 11,
		SCREEN = 12,
		BODEN = 13,
		WATERBOX = 14
	};

	std::vector<Model> get_model(ModelID id);
	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> shadercolor;
	std::shared_ptr<Shader> shaderpost;
	std::shared_ptr<Shader> shadertext;
	std::shared_ptr<Shader> shaderquad;
	std::shared_ptr<Shader> shaderpart;
	std::shared_ptr<Shader> shaderwater;
	std::shared_ptr<Shader> shadervideo;
};

