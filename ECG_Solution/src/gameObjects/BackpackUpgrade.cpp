#include "BackpackUpgrade.h"



BackpackUpgrade::BackpackUpgrade() : Item(BackpackUpgrade::value)
{
}


BackpackUpgrade::~BackpackUpgrade()
{
}

std::string BackpackUpgrade::toString()
{
	return BackpackUpgrade::toString(true);
}

std::string BackpackUpgrade::toString(bool is_static)
{
	return "backpack_upgrade";
}
