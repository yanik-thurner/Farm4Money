#pragma once
#include "Item.h"

class Fish :
	public Item
{
public:
	Fish();
	~Fish();

	static const int value = 50;
	// Inherited via Item
	virtual std::string toString() override;
	static std::string toString(bool is_static);
};

