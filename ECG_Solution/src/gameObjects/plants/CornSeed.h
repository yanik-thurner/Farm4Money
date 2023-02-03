#pragma once
#include "Seed.h"
class CornSeed : public Seed
{
public:
	CornSeed();
	~CornSeed();
	static const int value = 10;
	// Inherited via Seed
	virtual std::string toString() override;
	static std::string toString(bool is_static);
};


