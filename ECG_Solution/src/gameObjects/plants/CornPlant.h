#pragma once
#include "Plant.h"
#include "../Field.h"
#include "Corn.h"

class CornPlant :
	public Plant
{
public:
	CornPlant(Field *parent);
	~CornPlant();

	// Inherited via Plant
	virtual std::unique_ptr<Crop> harvest() override;
};

