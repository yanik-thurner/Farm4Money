#pragma once
#include "SceneObject.h"
#include "Player.h"

class Interactable : public SceneObject {
protected:
	Interactable(std::vector<Model> models, glm::mat4 transform_matrix, glm::vec3 position) : SceneObject(models, transform_matrix, position){}

public:
	bool playerInRange = false;
	virtual ~Interactable() {}
	virtual void interact(Player &p) = 0;
};