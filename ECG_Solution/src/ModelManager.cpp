#include "ModelManager.h"

ModelManager *ModelManager::m = 0;

ModelManager::ModelManager()
{
	shader = std::make_shared<Shader>("texture.vert", "toon.frag");
	shadercolor = std::make_shared<Shader>("texture.vert", "tooncolor.frag");
	shaderpost = std::make_shared<Shader>("outline.vert", "outline.frag");
	shadertext = std::make_shared<Shader>("textshader.vert", "textshader.frag");
	shaderquad = std::make_shared<Shader>("quadshader.vert", "quadshader.frag");
	shaderpart = std::make_shared<Shader>("particle.vert", "particle.frag");
	shaderwater = std::make_shared<Shader>("water.vert", "tooncolor.frag");
	shadervideo = std::make_shared<Shader>("video.vert", "video.frag");

	//std::shared_ptr<NewTexture> sunTexture = std::make_shared<NewTexture>("sun.dds");
	//std::shared_ptr<NewTexture> moonTexture = std::make_shared<NewTexture>("moon.dds");
	//std::shared_ptr<NewTexture> earthTexture = std::make_shared<NewTexture>("earth.dds");
	//std::shared_ptr<NewTexture> grassTexture = std::make_shared<NewTexture>("grass.dds");
	//std::shared_ptr<NewTexture> soilTexture = std::make_shared<NewTexture>("soil.dds");
	//std::shared_ptr<NewTexture> limeTexture = std::make_shared<NewTexture>("lime.dds");
	//std::shared_ptr<NewTexture> woodTexture = std::make_shared<NewTexture>("wood_texture.dds");
	//std::shared_ptr<NewTexture> lanternTexture = std::make_shared<NewTexture>("lantern.dds");

	// Create materials
	//std::shared_ptr<Material> sunMaterial = std::make_shared<TextureMaterial>(shader, glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, sunTexture);
	//std::shared_ptr<Material> earthMaterial = std::make_shared<TextureMaterial>(shader, glm::vec3(0.05f, 0.9f, 0.1f), 5.0f, earthTexture);
	//std::shared_ptr<Material> moonMaterial = std::make_shared<TextureMaterial>(shader, glm::vec3(0.05f, 0.9f, 0.05f), 2.0f, moonTexture);
	//std::shared_ptr<Material> grassMaterial = std::make_shared<TextureMaterial>(shader, glm::vec3(0.9f, 0.4f, 0.5f), 5.0f, grassTexture);
	//std::shared_ptr<Material> soilMaterial = std::make_shared<TextureMaterial>(shader, glm::vec3(0.5f, 0.9f, 0.05f), 2.0f, soilTexture);
	//std::shared_ptr<Material> plantMaterial = std::make_shared<TextureMaterial>(shader, glm::vec3(0.5f, 0.9f, 0.1f), 2.0f, limeTexture);
	//std::shared_ptr<Material> woodMaterial = std::make_shared<TextureMaterial>(shader, glm::vec3(0.5f, 0.9f, 0.1f), 5.0f, woodTexture);
	//std::shared_ptr<Material> lanternMaterial = std::make_shared<TextureMaterial>(shader, glm::vec3(0.5f, 0.9f, 0.1f), 5.0f, lanternTexture);

	// CUBE
	std::vector<ModelMetaData> cubes;
	Model cube = Model("assets/3d/cube.blend", shader, shadercolor);
	cubes.push_back(ModelMetaData("assets/3d/cube.blend",cube.getMeshes(),cube.getMat(), shader, shadercolor));

	// FIELD
	std::vector<ModelMetaData> fields;
	Model field = Model("assets/3d/field.blend", shader, shadercolor);
	fields.push_back(ModelMetaData("assets/3d/field.blend",field.getMeshes(),field.getMat(), shader, shadercolor));

	// LANTERN
	std::vector<ModelMetaData> lanterns;
	Model lantern = Model("assets/3d/lantern3.blend", shader, shadercolor);
	lanterns.push_back(ModelMetaData("assets/3d/lantern3.blend",lantern.getMeshes(),lantern.getMat(), shader, shadercolor));

	// PLAYER
	std::vector<ModelMetaData> players;
	Model player = Model("assets/3d/Farmer_Lowpoly3.blend", shader, shadercolor);
	players.push_back(ModelMetaData("assets/3d/Farmer_Lowpoly3.blend",player.getMeshes(),player.getMat(), shader, shadercolor));

	// SELLBOX
	std::vector<ModelMetaData> sellboxes;
	Model sellbox = Model("assets/3d/chest.blend", shader, shadercolor);
	sellboxes.push_back(ModelMetaData("assets/3d/chest.blend",sellbox.getMeshes(),sellbox.getMat(), shader, shadercolor));

	// CORNPLANT
	std::vector<ModelMetaData> cornplants;
	std::vector<std::shared_ptr<ModelMetaData>> cornplant_material_vector0;
	Model co1 = Model("assets/3d/corn1.blend", shader, shadercolor);
	Model co2 = Model("assets/3d/corn2.blend", shader, shadercolor);
	Model co3 = Model("assets/3d/corn3.blend", shader, shadercolor);
	cornplants.push_back(ModelMetaData("assets/3d/corn1.blend",co1.getMeshes(),co1.getMat(), shader, shadercolor));
	cornplants.push_back(ModelMetaData("assets/3d/corn2.blend",co2.getMeshes(), co2.getMat(), shader, shadercolor));
	cornplants.push_back(ModelMetaData("assets/3d/corn3.blend", co3.getMeshes(), co3.getMat(), shader, shadercolor));
	
	// CARROTPLANT
	std::vector<ModelMetaData> carrotplants;
	std::vector<std::shared_ptr<Material>> carrotplant_material_vector0;
	Model ca1 = Model("assets/3d/carrot1.blend", shader, shadercolor);
	Model ca2 = Model("assets/3d/carrot2.blend", shader, shadercolor);
	Model ca3 = Model("assets/3d/carrot3.blend", shader, shadercolor);
	carrotplants.push_back(ModelMetaData("assets/3d/carrot1.blend",ca1.getMeshes(),ca1.getMat(), shader, shadercolor));
	carrotplants.push_back(ModelMetaData("assets/3d/carrot2.blend", ca2.getMeshes(), ca2.getMat(), shader, shadercolor));
	carrotplants.push_back(ModelMetaData("assets/3d/carrot3.blend", ca3.getMeshes(), ca3.getMat(), shader, shadercolor));

	// MELONPLANT
	std::vector<ModelMetaData> melonplants;
	std::vector<std::shared_ptr<Material>> melonplant_material_vector0;
	Model mel1 = Model("assets/3d/melon1.blend", shader, shadercolor);
	Model mel2 = Model("assets/3d/melon2.blend", shader, shadercolor);
	Model mel3 = Model("assets/3d/melon3.blend", shader, shadercolor);
	melonplants.push_back(ModelMetaData("assets/3d/melon1.blend",mel1.getMeshes(),mel1.getMat(), shader, shadercolor));
	melonplants.push_back(ModelMetaData("assets/3d/melon2.blend", mel2.getMeshes(), mel2.getMat(), shader, shadercolor));
	melonplants.push_back(ModelMetaData("assets/3d/melon3.blend", mel3.getMeshes(), mel3.getMat(), shader, shadercolor));

	// TOMATOPLANT
	std::vector<ModelMetaData> tomatoplants;
	std::vector<std::shared_ptr<Material>> tomatoplant_material_vector0;
	Model tom1 = Model("assets/3d/tomato1.blend", shader, shadercolor);
	Model tom2 = Model("assets/3d/tomato2.blend", shader, shadercolor);
	Model tom3 = Model("assets/3d/tomato3.blend", shader, shadercolor);
	tomatoplants.push_back(ModelMetaData("assets/3d/tomato1.blend", tom1.getMeshes(),tom1.getMat(),shader, shadercolor));
	tomatoplants.push_back(ModelMetaData("assets/3d/tomato2.blend", tom2.getMeshes(), tom2.getMat(), shader, shadercolor));
	tomatoplants.push_back(ModelMetaData("assets/3d/tomato3.blend", tom3.getMeshes(), tom3.getMat(), shader, shadercolor));

	// SHOP
	std::vector<ModelMetaData> shops;
	Model shop = Model("assets/3d/chest.blend", shader, shadercolor);
	shops.push_back(ModelMetaData("assets/3d/chest.blend",shop.getMeshes(),shop.getMat(), shader, shadercolor));

	// SALESIGN
	std::vector<ModelMetaData> salesigns;
	Model ssing = Model("assets/3d/salesign.blend", shader, shadercolor);
	salesigns.push_back(ModelMetaData("assets/3d/salesign.blend",ssing.getMeshes(), ssing.getMat(), shader, shadercolor));

	// WATER
	std::vector<ModelMetaData> waters;
	Model water = Model("assets/3d/water.blend", shader, shaderwater);
	waters.push_back(ModelMetaData("assets/3d/water.blend",water.getMeshes(),water.getMat(), shader, shaderwater));

	// SCREEN
	std::vector<ModelMetaData> screens;
	Model screen = Model("assets/3d/screen.blend", shadervideo, shadervideo);
	screens.push_back(ModelMetaData("assets/3d/screen.blend", screen.getMeshes(), screen.getMat(), shadervideo, shadervideo));

	// BODEN
	std::vector<ModelMetaData> bodens;
	Model boden = Model("assets/3d/boden.blend", shader, shadercolor);
	bodens.push_back(ModelMetaData("assets/3d/screen.blend", boden.getMeshes(), boden.getMat(), shader, shadercolor));
	
	// WATERBOX
	std::vector<ModelMetaData> waterboxs;
	Model waterbox = Model("assets/3d/waterbox.blend", shader, shadercolor);
	waterboxs.push_back(ModelMetaData("assets/3d/waterbox.blend", waterbox.getMeshes(), waterbox.getMat(), shader, shadercolor));

	model_vectors.push_back(cubes);
	model_vectors.push_back(fields);
	model_vectors.push_back(lanterns);
	model_vectors.push_back(players);
	model_vectors.push_back(sellboxes);
	model_vectors.push_back(cornplants);
	model_vectors.push_back(carrotplants);
	model_vectors.push_back(melonplants);
	model_vectors.push_back(tomatoplants);
	model_vectors.push_back(shops);
	model_vectors.push_back(salesigns);
	model_vectors.push_back(waters);
	model_vectors.push_back(screens);
	model_vectors.push_back(bodens);
	model_vectors.push_back(waterboxs);

}


ModelManager::~ModelManager()
{
}

ModelManager * ModelManager::instance()
{
	if (m == nullptr) {
		m = new ModelManager;
	}
	return m;
}

std::vector<Model> ModelManager::get_model(ModelManager::ModelID id)
{
	std::vector<Model> models;
	for (int i = 0; i < model_vectors[id].size(); i++) {
		//models.push_back(Model(model_vectors[id][i].file_path, model_vectors[id][i].s1, model_vectors[id][i].s2));
		models.push_back(Model(model_vectors[id][i].meshes, model_vectors[id][i].matvec, model_vectors[id][i].s1, model_vectors[id][i].s2));
	}
	return models;
}

