#include "CarrotSeed.h"



CarrotSeed::CarrotSeed() : Seed(CarrotSeed::value)
{
}


CarrotSeed::~CarrotSeed()
{
}

std::string CarrotSeed::toString()
{
	return CarrotSeed::toString(true);
}

std::string CarrotSeed::toString(bool is_static)
{
	return "carrot_seed";
}
