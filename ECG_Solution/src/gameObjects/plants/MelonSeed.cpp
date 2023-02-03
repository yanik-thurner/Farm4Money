#include "MelonSeed.h"



MelonSeed::MelonSeed() : Seed(MelonSeed::value)
{
}


MelonSeed::~MelonSeed()
{
}

std::string MelonSeed::toString()
{
	return  MelonSeed::toString(true);
}

std::string MelonSeed::toString(bool is_static)
{
	return "melon_seed";
}
