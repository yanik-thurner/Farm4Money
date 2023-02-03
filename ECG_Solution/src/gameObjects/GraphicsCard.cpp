#include "GraphicsCard.h"



GraphicsCard::GraphicsCard() : Item(GraphicsCard::value)
{
}


GraphicsCard::~GraphicsCard()
{
}

std::string GraphicsCard::toString()
{
	return GraphicsCard::toString(true);
}

std::string GraphicsCard::toString(bool is_static)
{
	return "graphics_card";
}
