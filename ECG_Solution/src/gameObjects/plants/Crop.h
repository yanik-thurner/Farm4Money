#pragma once
#include "../Item.h"

class Crop : public Item
{
protected:
	Crop(int value) : Item(value) {};
public:
	virtual ~Crop() {}
};

