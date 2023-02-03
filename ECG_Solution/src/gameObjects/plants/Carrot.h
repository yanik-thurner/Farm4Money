#pragma once
#include "Crop.h"

class Carrot : public Crop
{
public:
	Carrot();
	~Carrot();
	static const int value = 80;
	// Inherited via Crop
	virtual std::string toString() override;
	static std::string toString(bool is_static);
};

