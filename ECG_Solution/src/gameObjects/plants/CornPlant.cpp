#include "CornPlant.h"


CornPlant::CornPlant(Field *parent) : Plant(parent, SceneObject::model_manager->get_model(ModelManager::CORNPLANT))
{
	level_times.push_back(4);
	level_times.push_back(4);
	level_times.push_back(4);
}


CornPlant::~CornPlant()
{
}

std::unique_ptr<Crop> CornPlant::harvest()
{
	std::unique_ptr<Crop> c = std::make_unique<Corn>();
	return c;
}
