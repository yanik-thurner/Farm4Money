#pragma once
#include "Plant.h"
#include "Melon.h"

class MelonPlant :
	public Plant
{
public:
	MelonPlant(Field *parent);
	~MelonPlant();

	// Inherited via Plant
	virtual std::unique_ptr<Crop> harvest() override;
};

