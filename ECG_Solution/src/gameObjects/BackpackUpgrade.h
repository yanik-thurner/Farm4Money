#pragma once
#include "Item.h"
class BackpackUpgrade :
	public Item
{
public:
	BackpackUpgrade();
	~BackpackUpgrade();

	static const int value = 500;
	// Inherited via Item
	virtual std::string toString() override;
	static std::string toString(bool is_static);
};

