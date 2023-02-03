#pragma once
#include "Seed.h"
class MelonSeed :
	public Seed
{
public:
	MelonSeed();
	~MelonSeed();
	static const int value = 20;
	// Inherited via Seed
	virtual std::string toString() override;
	static std::string toString(bool is_static);
};

