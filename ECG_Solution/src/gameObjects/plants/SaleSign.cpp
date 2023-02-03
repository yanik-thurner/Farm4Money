#include "SaleSign.h"



SaleSign::SaleSign(Field *parent) : Plant(parent, SceneObject::model_manager->get_model(SceneObject::model_manager->SALESIGN))
{
}


SaleSign::~SaleSign()
{
}

std::unique_ptr<Crop> SaleSign::harvest()
{
	return std::unique_ptr<Crop>();
}

bool SaleSign::is_maxed()
{
	return true;
}
