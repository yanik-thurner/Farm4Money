#pragma once
#include "Interactable.h"
#include "Item.h"
#include "Collidable.h"

class SellBox :
	public Interactable, public Collidable
{
private:
	static physx::PxShape* shape;
public:
	SellBox(glm::vec3 size, glm::vec3 position);
	~SellBox();

	int sell(std::unique_ptr<Item> i);

	// Inherited via Interactable
	virtual void apply_model() override;
	virtual void update(double timediff, glm::vec3 dir) override;
	virtual void draw(glm::vec3 cam_pos, float view_frustum) override;

	virtual void interact(Player &p) override;


};

