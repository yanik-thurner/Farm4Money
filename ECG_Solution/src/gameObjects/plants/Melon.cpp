#include "Melon.h"



Melon::Melon() : Crop(Melon::value)
{
}


Melon::~Melon()
{
}


std::string Melon::toString()
{
	return Melon::toString(true);
}

std::string Melon::toString(bool is_static)
{
	return "melon";
}
