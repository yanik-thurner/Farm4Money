#pragma once
#include "Crop.h"

class Corn : public Crop
{
public:
	Corn();
	~Corn();
	static const int value = 50;
	// Inherited via Crop
	virtual std::string toString() override;
	static std::string toString(bool is_static);
};

