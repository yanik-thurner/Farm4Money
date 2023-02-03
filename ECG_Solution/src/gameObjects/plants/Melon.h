#pragma once
#include "Crop.h"
class Melon :
	public Crop
{
public:
	Melon();
	~Melon();
	static const int value = 150;
	// Inherited via Crop
	virtual std::string toString() override;
	static std::string toString(bool is_static);
};

