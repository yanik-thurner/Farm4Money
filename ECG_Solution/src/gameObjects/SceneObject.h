#pragma once
#include "../Geometry.h"
#include "../Utils.h"
#include "../Model.h"
#include "PhysX\PxPhysicsAPI.h"
#include "../ModelManager.h"
#include "../TextRender.h"

class Player;
class SceneObject
{
protected:
	SceneObject(std::vector<Model> models, glm::mat4 transform_matrix, glm::vec3 position) : idle_model(models[0]), current_model(models[0]), models(models), _position(position) {
		this->current_model = current_model;
		this->transform_matrix = transform_matrix;
	}

	std::vector<Model> models;
	Model idle_model;
	Model current_model;
	physx::PxRigidActor *actor = nullptr;
	

	virtual void apply_model() = 0;

	void set_actor(physx::PxRigidActor *actor) {
		if (this->actor) {
			gScene->removeActor(*this->actor);
		}

		this->actor = actor;
		gScene->addActor(*actor);
	}

public:
	static physx::PxPhysics* gPhysicsSDK;
	static ModelManager *model_manager;
	static physx::PxScene *gScene;
	static physx::PxCooking *cooking;
	glm::vec3 _position;

	virtual ~SceneObject() {
	}
	glm::mat4 transform_matrix;
	
	

	virtual void update(double timediff, glm::vec3 dir) = 0;
	virtual void draw(glm::vec3 cam_pos, float view_frustum) = 0;
};

