#pragma once
#include "Interactable.h"
#include "..\Model.h"
#include "Item.h"
#include "plants\Plant.h"
#include <vector>
#include <memory>
#include "Collidable.h"

class Plant;
class Field : public Interactable, public Collidable
{
private:
	static physx::PxShape* shape;
protected:
	// Inherited via Interactable
	virtual void apply_model() override;

public:
	Field(glm::vec3 size, glm::vec3 position);
	~Field();

	glm::vec3 pos;
	std::unique_ptr<Plant> plant;

	void plant_a_seed(Player &p);
	void harvest(Player &p);

	// Inherited via SceneObject
	virtual void update(double timediff, glm::vec3 dir) override;
	virtual void draw(glm::vec3 cam_pos, float view_frustum) override;

	// Inherited via Interactable
	virtual void interact(Player &p) override;


	
};

