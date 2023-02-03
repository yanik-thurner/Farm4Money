#pragma once
#include "Seed.h"
class CarrotSeed :
	public Seed
{
public:
	CarrotSeed();
	~CarrotSeed();
	static const int value = 15;
	// Inherited via Seed
	virtual std::string toString() override;
	static std::string toString(bool is_static);
};

