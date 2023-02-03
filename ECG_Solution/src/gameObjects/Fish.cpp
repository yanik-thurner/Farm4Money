#include "Fish.h"



Fish::Fish() : Item(Fish::value)
{
}


Fish::~Fish()
{
}
std::string Fish::toString()
{
	return Fish::toString(true);
}

std::string Fish::toString(bool is_static)
{
	return "fish";
}