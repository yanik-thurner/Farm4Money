#include "TomatoPlant.h"



TomatoPlant::TomatoPlant(Field *parent) : Plant(parent, SceneObject::model_manager->get_model(ModelManager::TOMATOPLANT))
{
	level_times.push_back(3);
	level_times.push_back(3);
	level_times.push_back(3);
}


TomatoPlant::~TomatoPlant()
{
}

std::unique_ptr<Crop> TomatoPlant::harvest()
{
	std::unique_ptr<Crop> c = std::make_unique<Tomato>();
	return c;
}
