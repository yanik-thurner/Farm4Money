#include "Tomato.h"



Tomato::Tomato() : Crop(Tomato::value)
{
}


Tomato::~Tomato()
{
}

std::string Tomato::toString()
{
	return Tomato::toString(true);
}

std::string Tomato::toString(bool is_static)
{
	return "tomato";
}
