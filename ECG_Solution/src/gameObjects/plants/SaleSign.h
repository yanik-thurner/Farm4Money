#pragma once
#include "../Field.h"

class SaleSign :
	public Plant
{
public:
	SaleSign(Field* parent);
	~SaleSign();

	static const int price = 250;

	// Inherited via Plant
	virtual std::unique_ptr<Crop> harvest() override;
	virtual bool is_maxed() override;
};

