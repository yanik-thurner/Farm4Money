#pragma once
#include "Crop.h"
class Tomato :
	public Crop
{
public:
	Tomato();
	~Tomato();
	static const int value = 25;
	// Inherited via Crop
	virtual std::string toString() override;
	static std::string toString(bool is_static);
};

