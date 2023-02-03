#include "Carrot.h"

Carrot::Carrot() : Crop(Carrot::value)
{
}


Carrot::~Carrot()
{
}

std::string Carrot::toString()
{
	return Carrot::toString(true);
}

std::string Carrot::toString(bool is_static)
{
	return  "carrot";
}
