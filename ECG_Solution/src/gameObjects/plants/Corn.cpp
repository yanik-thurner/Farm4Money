#include "Corn.h"

Corn::Corn() : Crop(Corn::value)
{
}


Corn::~Corn()
{
}

std::string Corn::toString()
{
	return Corn::toString(true);
}

std::string Corn::toString(bool is_static)
{
	return "corn";
}
