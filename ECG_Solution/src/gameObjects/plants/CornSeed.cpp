#include "CornSeed.h"

CornSeed::CornSeed() : Seed(CornSeed::value)
{
}


CornSeed::~CornSeed()
{
}

std::string CornSeed::toString()
{
	return CornSeed::toString(true);
}

std::string CornSeed::toString(bool is_static)
{
	return "corn_seed";
}
