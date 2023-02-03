#pragma once
#include "Item.h"
class GraphicsCard :
	public Item
{
public:
	GraphicsCard();
	~GraphicsCard();

	static const int value = 5000;
	// Inherited via Item
	virtual std::string toString() override;
	static std::string toString(bool is_static);

};

