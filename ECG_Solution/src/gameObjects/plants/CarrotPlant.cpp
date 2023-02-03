#include "CarrotPlant.h"



CarrotPlant::CarrotPlant(Field *parent) : Plant(parent, SceneObject::model_manager->get_model(ModelManager::CARROTPLANT))
{
	level_times.push_back(6);
	level_times.push_back(6);
	level_times.push_back(6);
}


CarrotPlant::~CarrotPlant()
{
}

std::unique_ptr<Crop> CarrotPlant::harvest()
{
	std::unique_ptr<Crop> c = std::make_unique<Carrot>();
	return c;
}
