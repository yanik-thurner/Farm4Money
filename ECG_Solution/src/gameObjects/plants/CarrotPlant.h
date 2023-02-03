#pragma once
#include "Plant.h"
#include "../Field.h"
#include "Carrot.h"

class CarrotPlant :
	public Plant
{
public:
	CarrotPlant(Field  *parent);
	~CarrotPlant();

	// Inherited via Plant
	virtual std::unique_ptr<Crop> harvest() override;
};

