#pragma once
#include "Seed.h"
class TomatoSeed :
	public Seed
{
public:
	TomatoSeed();
	~TomatoSeed();
	static const int value = 5;
	// Inherited via Seed
	virtual std::string toString() override;
	static std::string toString(bool is_static);
};

