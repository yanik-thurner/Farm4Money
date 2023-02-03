#pragma once
#include "Plant.h"
#include "Tomato.h"

class TomatoPlant :
	public Plant
{
public:
	TomatoPlant(Field *parent);
	~TomatoPlant();

	// Inherited via Plant
	virtual std::unique_ptr<Crop> harvest() override;
};

