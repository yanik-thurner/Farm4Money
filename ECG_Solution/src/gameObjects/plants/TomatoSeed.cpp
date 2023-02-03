#include "TomatoSeed.h"



TomatoSeed::TomatoSeed() : Seed(TomatoSeed::value)
{
}


TomatoSeed::~TomatoSeed()
{
}

std::string TomatoSeed::toString()
{
	return TomatoSeed::toString(true);
}

std::string TomatoSeed::toString(bool is_static)
{
	return "tomato_seed";
}
