#include "MelonPlant.h"



MelonPlant::MelonPlant(Field *parent) : Plant(parent, SceneObject::model_manager->get_model(ModelManager::MELONPLANT))
{
	level_times.push_back(10);
	level_times.push_back(10);
	level_times.push_back(10);
}


MelonPlant::~MelonPlant()
{
}

std::unique_ptr<Crop> MelonPlant::harvest()
{
	std::unique_ptr<Crop> c = std::make_unique<Melon>();
	return c;
}
