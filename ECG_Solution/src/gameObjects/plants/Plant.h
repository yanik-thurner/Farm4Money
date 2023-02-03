#pragma once
#include "../SceneObject.h"
#include "../Field.h"
#include "../../Particle.h"

class Field;
class Plant :
	public SceneObject
{
protected:
	int plantState = 0;
	double total_grow_time = 0;
	std::vector<int> level_times;
	Particle particle_system;

	// Inherited via Interactable
	virtual void apply_model() override;

	Plant(Field *parent, std::vector<Model> models);
public:
	virtual bool is_maxed() {
		return plantState == (level_times.size() - 1);
	}

	void grow();
	virtual std::unique_ptr<Crop> harvest() = 0;
	virtual void update(double timediff, glm::vec3 dir) override;
	virtual void draw(glm::vec3 cam_pos, float view_frustum) override;
};

